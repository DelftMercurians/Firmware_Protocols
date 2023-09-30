#include "serial_interface.h"

SerialInterface::SerialInterface(Stream* s, char name) {
    this->s = s;
    this->name = name;
}

void SerialInterface::initFuns() {
    this->add('?', &SerialInterface::printHelp, this, "Print commands");
    this->add('e', SerialInterface::echo, "Echo");
    #ifdef VERSION_SHORT
    this->add('v', SerialInterface::printVersion, "Version");
    #endif
    #ifdef PROTOCOL_VERSION
    this->add('#', SerialInterface::printProtocolVersion, "Protocol version");
    #endif
}

void SerialInterface::add(char command, void (*function) (char*), String help) {
    Function fun;
    fun.typ = FunctionType::CharPointer;
    fun.fun_charptr = function;
    fun.help = help;
    fun.subcommand = false;
    fun.si = this; // Just in case
    fun_map[command] = fun;
}


void SerialInterface::add(char command, void (*function) (float), String help) {
    Function fun;
    fun.typ = FunctionType::Float;
    fun.fun_float = function;
    fun.help = help;
    fun.subcommand = false;
    fun.si = this; // Just in case
    fun_map[command] = fun;
}

void SerialInterface::readFloatAndRun(char* c, void (*function) (float)) {
    float f = atof(c);
    function(f);
}

void SerialInterface::setFloat(char* c, float* f) {
    *f = atof(c);
}

void SerialInterface::add(char command, void (*function) (int), String help) {
    Function fun;
    fun.typ = FunctionType::Int;
    fun.fun_int = function;
    fun.help = help;
    fun.subcommand = false;
    fun.si = this; // Just in case
    fun_map[command] = fun;
}

void SerialInterface::readIntAndRun(char* c, void (*function) (int)) {
    int i = atoi(c);
    function(i);
}

void SerialInterface::setInt(char* c, int* i) {
    *i = atoi(c);
}

void SerialInterface::add(char command, float* val, String help) {
    Function fun;
    fun.typ = FunctionType::SetFloat;
    fun.ref_float = val;
    fun.help = help;
    fun.subcommand = false;
    fun.si = this; // Just in case
    fun_map[command] = fun;
}

void SerialInterface::add(char command, int* val, String help) {
    Function fun;
    fun.typ = FunctionType::SetInt;
    fun.ref_int = val;
    fun.help = help;
    fun.subcommand = false;
    fun.si = this; // Just in case
    fun_map[command] = fun;
}

// void SerialInterface::add(char command, void (*function) (float*, size_t), String help) {
//     Function fun;
//     fun.typ = FunctionType::FloatPointer;
//     fun.fun_floatptr = function;
//     fun.help = help;
//     fun.subcommand = false;
//     fun.si = this; // Just in case
//     fun_map[command] = fun;
//     Serial.println("Adding function.");
// }

// void SerialInterface::readFloatsAndRun(char* c, void (*function) (float*, size_t)) {
//     float f[20];
//     uint8_t i;
//     for(i = 0; i < 20; i++){
// 		f[i] = atof(c);
// 		while(*(c++) == ' '){}
// 		while(*c != ' '){
// 			if(*c == '\n' || *c == '\r' || *c == 0){
// 				goto run;
// 			}
// 			c++;
// 		}
// 		c++;
// 	}
//     run:
//     function(f, i);
// }


void SerialInterface::add(char command, void (SerialInterface::*mem_function) (char*), SerialInterface* si, String help) {
    Function fun;
    fun.typ = FunctionType::Member_CharPointer;
    fun.fun_member_charptr = mem_function;
    fun.si = si;
    fun.help = help;
    fun.subcommand = false;
    fun_map[command] = fun;
}

void SerialInterface::add(char command, SerialInterface* si, String help) {
    this->add(command, &SerialInterface::run, si, help);
    fun_map[command].subcommand = true;
}


void SerialInterface::printHelp(char* c) {
    printHelp();
}

void SerialInterface::printHelp(String indentation) {
    if(indentation == ""){
        Serial.println("Commands:");
    }
    for (auto const& it : fun_map){
        Function fun = it.second;
        Serial.print(indentation);
        Serial.printf("[%c] ", it.first);
        Serial.println(fun.help);
        if(fun.subcommand){
            fun.si->printHelp(indentation + String("  "));
        }
    }
}


void SerialInterface::run(char* c) {
    if(isTerminator(c[0])) return;

    auto item = fun_map.find(c[0]);
    if(item == fun_map.end()) {
        // Command is not known
        Serial.printf("Unknown command: [%c]\n", c[0]);
        return;
    }

    // Serial.printf("[%c] Running command [%c]\n", name, c[0]);
    Function fun = item->second;
    switch(fun.typ){
        case FunctionType::CharPointer:
            fun.fun_charptr(c+1);
            break;
        case FunctionType::Member_CharPointer:
            ((*fun.si).*(fun.fun_member_charptr))(c+1); // Sorcery
            break;
        case FunctionType::Float:
            readFloatAndRun(c+1, fun.fun_float);
            break;
        // case FunctionType::FloatPointer:
        //     readFloatsAndRun(c+1, fun.fun_floatptr);
        //     break;
        case FunctionType::Int:
            readIntAndRun(c+1, fun.fun_int);
            break;
        case FunctionType::SetFloat:
            setFloat(c+1, fun.ref_float);
            break;
        case FunctionType::SetInt:
            setInt(c+1, fun.ref_int);
            break;
        default:
            Serial.println("Unhandled FunctionType");
    }
    
    
}

void SerialInterface::run() {
    if(s == nullptr) return;
    while(s->available() && bufi < 50){
        char c = s->read();
        buffer[bufi++] = c;
        if(isTerminator(c)){
            run(buffer);
            bufi = 0;
        }
    }
    if(bufi == 50){
        bufi = 0;
    }
}


bool SerialInterface::isTerminator(char c) {
    return (c == 0) || (c == '\n') || (c == '\r') || (c == ';');
}

void SerialInterface::echo(char* c) {
	while(*c != '\n' && *c != '\r' && *c != 0) {
		Serial.print(*c);
		c++;
	}
	Serial.println();
}


void SerialInterface::printVersion(char* c) {
    #ifdef VERSION_SHORT
    Serial.print(VERSION_SHORT);
    Serial.print('\n');
    #endif
}

void SerialInterface::printProtocolVersion(char* c) {
    #ifdef PROTOCOL_VERSION
    Serial.print(PROTOCOL_VERSION);
    Serial.print('\n');
    #endif
}

