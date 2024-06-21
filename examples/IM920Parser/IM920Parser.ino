#include "IM920Parser.h"

IM920Parser IM920sL;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  uint8_t data[] = {16, 32, 26, 5};
  const char* msg = IM920sL.setCommandBroadcast(data, 4, false);
  Serial.println(msg);
  delete msg;

  // uint8_t data2[] = {'A', 'B', '8', 'C'};
  // const char* msg2 = IM920sL.setCommandBroadcast(data2, 4, true);
  // Serial.println(msg2);
  // delete msg2;
  
  // uint8_t data3[] = {16, 32, 26, 5, 128, 63};
  // const char* msg3 = IM920sL.setCommandUnicast(10, data3, 6, false);
  // Serial.println(msg3);
  // delete msg3;
  
  // uint8_t data4[] = {'A', 'B', '8', 'C'};
  // const char* msg4 = IM920sL.setCommandUnicast(125, data4, 4, true);
  // Serial.println(msg4);
  // delete msg4;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    String str = Serial.readStringUntil('\n') + '\n'; // manually adding LF('\n) because it's removed from the original message
    //Serial.println(str);

    // analyzing process
    auto res = IM920sL.process(str.c_str(), str.length());

    if(res < 0){
      // error handling
    }
    else{
      Serial.println("Node: " + String(IM920sL.getSenderID()) + ", RSSI: " + String(IM920sL.getRssi()));
      Serial.println(IM920sL.getPacketData(0));
    }
  }
}
