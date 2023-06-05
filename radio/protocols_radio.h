// Delft Mercurians
// Thomas Hettasch
// 2023-05-18
// thomas.hettasch@gmail.com

#pragma once
namespace Radio {
    
enum class Status {
    EMERGENCY,
    OK,
    STOP,
    STARTING,
    NO_REPLY,
};

struct Command {
    float x;
    float y;
    float z;
};

struct Reply {
    Status status;
};

}