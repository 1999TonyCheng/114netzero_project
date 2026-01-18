#ifndef RF_CONTROL_H
#define RF_CONTROL_H

#include <RCSwitch.h>

struct FloorCommand {
  String floor;
  unsigned long lightCode;
  unsigned long fanBaseCode;
};

class RFControl {
  public:
    RFControl(int txPin = 5, int protocol = 11, int pulse =320);
    bool sendLight(const String& floor, int times);
    bool sendFan(const String& floor, int level);
    FloorCommand* getCommandForFloor(const String& floor);
    FloorCommand* getAllCommands();
    int getCommandCount();

  private:
    RCSwitch mySwitch;
    FloorCommand floorCommands[4] = {
      {"客廳", 424218, 424192},
      {"廚房", 886810, 886784},
      {"2F", 1203994, 1203968},
      {"3F", 16776730, 16776704}
    };
};

#endif