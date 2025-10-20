#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

TaskHandle_t thp[1];//マルチスレッドのタスクハンドル格納用

void setup() {
  Serial.begin(9600);//シリアル通信のボートレートを設定
  //Serial2.begin(9600);  
  SerialBT.begin("MOTOR");//Bluetooth通信の名前を設定

  xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 3, &thp[0], 0);//Core0でのスレッド作成
}

void loop() { 
  if (Serial.available()) {//データを来たら
    String sendData = Serial.readStringUntil(';');//;が来るまで文字列を記録
    SerialBT.print(sendData);//文字列を送信
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

