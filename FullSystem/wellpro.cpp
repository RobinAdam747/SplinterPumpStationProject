// wellpro.cpp
#include "wellpro.h"

bool readWellPro() {
  uint8_t result = node.readInputRegisters(0x0001, 8);
  if (result == node.ku8MBSuccess) {
    for (int i = 0; i < 8; i++) {
      digitalInput[i] = node.getResponseBuffer(i);
    }
    return true;
  }
  return false;
}
