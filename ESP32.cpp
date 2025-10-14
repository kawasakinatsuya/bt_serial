#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); 
  delay(1000);
  Serial.println("Bluetooth started");
}

void loop() {
  // put your main code here, to run repeatedly:
  //SerialBT.println("Hello");
  if (SerialBT.available()) {
    String msg = SerialBT.readStringUntil(';');
    Serial.print("Received via BT: ");
    Serial.println(msg);
  }
  

  delay(100);

}
