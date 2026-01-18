#include "RFControl.h"

RFControl::RFControl(int txPin, int protocol, int pulse) {
  // 發射腳位
  mySwitch.enableTransmit(txPin);
  // 設定 protocol
  mySwitch.setProtocol(protocol);
  // 設定 pulse 長度
  mySwitch.setPulseLength(pulse);
}

FloorCommand* RFControl::getCommandForFloor(const String& floor) {
  for (auto& cmd : floorCommands) {
    if (cmd.floor == floor) return &cmd;
  }
  return nullptr;
}

bool RFControl::sendLight(const String& floor, int times) {
  FloorCommand* cmd = getCommandForFloor(floor);
  if (!cmd) return false;
  for (int i = 0; i < times; ++i) {
    mySwitch.send(cmd->lightCode, 24);
    delay(300);
  }
  return true;
}

bool RFControl::sendFan(const String& floor, int level) {
  FloorCommand* cmd = getCommandForFloor(floor);
  if (!cmd) return false;
  unsigned long fanCode = (level == 0) ? cmd->fanBaseCode : (cmd->fanBaseCode + 7 - level);
  mySwitch.send(fanCode, 24);
  return true;
}

FloorCommand* RFControl::getAllCommands() {
  return floorCommands;
}

int RFControl::getCommandCount() {
  return sizeof(floorCommands) / sizeof(FloorCommand);
}