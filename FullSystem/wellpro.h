// wellpro.h
#ifndef WELLPRO_H
#define WELLPRO_H

#include <ModbusMaster.h>
extern ModbusMaster node; // Declare the ModbusMaster instance as external
extern float digitalInput[8]; // Declare the digitalInput array as external

bool readWellPro();

#endif // WELLPRO_H
