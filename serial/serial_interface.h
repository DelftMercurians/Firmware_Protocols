#pragma once
#include <map>
#include <Arduino.h>

class SerialInterface {
    public:
        SerialInterface(Stream* s = nullptr, char name = 'm');

        void initFuns();

        void add(char command, void (*function) (char*), String help = "");
        void add(char command, void (*function) (float), String help = "");
        void add(char command, void (*function) (int), String help = "");
        // void add(char command, void (*function) (float*, size_t), String help = "");

        void add(char command, void (SerialInterface::*) (char*), SerialInterface*, String help = "");
        void add(char command, SerialInterface*, String help = "");
        void run(char* c);
        void run();
        
    protected:
        void printHelp(char* c);
        void printHelp(String indentation = "");
        std::map<char, String> help_map;

        void readFloatAndRun(char* c, void (*function) (float));
        // void readFloatsAndRun(char* c, void (*function) (float*, size_t));
        void readIntAndRun(char* c, void (*function) (int));
    private:
        Stream* s;

        char buffer[50];
        size_t bufi;

        bool isTerminator(char c);

        char name;

        static void echo(char* c);

        enum class FunctionType {
            CharPointer,
            Member_CharPointer,
            Float,
            Int,
            // FloatPointer,
        };

        struct Function {
            FunctionType typ;
            union {
                void (*fun_charptr) (char*);
                void (SerialInterface::*fun_member_charptr) (char*);
                void (*fun_float) (float);
                void (*fun_int) (int);
                // void (*fun_floatptr) (float*, size_t);
            };
            String help;
            SerialInterface* si;
            bool subcommand;
        };

        std::map<char, Function> fun_map;



};