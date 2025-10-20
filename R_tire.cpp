#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

TaskHandle_t thp[1];//マルチスレッドのタスクハンドル格納用

void setup() {
  Serial.begin(115200);
  //Serial2.begin(115200);  
  SerialBT.begin("R_TIRE");

  xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 3, &thp[0], 0); 
}

void loop() { 
  if (Serial.available()) {
    String sendData = Serial.readStringUntil(';');
    SerialBT.print(sendData); 
    //Serial.print(sendData);
  }
}

void Core0a(void *args) {
  while (1) {
    delay(1);//サブで実行するプログラムを書く
      if (SerialBT.available()) {
        String receiveData = SerialBT.readStringUntil(';');
        //Serial2.print(receiveData);
        Serial.print(receiveData);
      }
  }
}

