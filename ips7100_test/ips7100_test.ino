#include <HardwareSerial.h>
#include <Wire.h>
#include "EEPROM.h"

#define Txd               1
#define Rxd               3
#define rts_485           26

#define powerControl      15

#define Sensor_Select_A   4
#define Sensor_Select_B   5

#define sensor_Txd        33        //
#define sensor_Rxd        35        //

#define     rxPM_buffer_ActiveMax   24
char        rxPM_buffer[rxPM_buffer_ActiveMax];

HardwareSerial  sensorSerial(1);


void setup() {
  Serial.begin(115200);
  sensorSerial.begin(9600, SERIAL_8N1, sensor_Rxd, sensor_Txd);
//  sensorSerial.begin(9600, SERIAL_8N1, Rxd, Txd);
  setPinMode();


  
  

  xTaskCreate(
    taskSensor,
    "rs485_task",
    10000,
    NULL,
    1,
    NULL
  );

  printf("Setup\n");
}

void loop() {

  delay(1000);
}


void setPinMode() {
  pinMode(powerControl, OUTPUT);
  pinMode(Sensor_Select_A, OUTPUT);
  pinMode(Sensor_Select_B, OUTPUT);

  digitalWrite(powerControl, HIGH);     //맨 처음 파워를 ON한다.  5V
  digitalWrite(Sensor_Select_A, HIGH);
  digitalWrite(Sensor_Select_B, LOW);
}


void taskSensor(void* param) {
  while(1) {
    sensorRead();

    delay(100);
  }
}

void sensorRead(){
  unsigned short us_cnt       = 0;
  unsigned short us_readCnt   = 0;
  printf("Hear!\n");
  if(sensorSerial.available() > 0) {
    us_readCnt = sensorSerial.available();
    delay(50);
    
    while(sensorSerial.available() > 0) {
      if(sensorSerial.available() > us_readCnt) {
        us_readCnt = sensorSerial.available();
        delay(50);
      } else if(sensorSerial.available() == us_readCnt) {
        break;
      }
    }
  }

  while(us_readCnt > 0) {
    byte ch = sensorSerial.read();
    rxPM_buffer[us_cnt] = ch;
    
    us_cnt++;
    if(us_cnt >= rxPM_buffer_ActiveMax) break;
    if(us_readCnt == us_cnt) break;
  }

  if(us_cnt > 0) {
    for(int i=0; i<us_cnt; i++) {
      printf("%02X ", rxPM_buffer[i]);
      if(i == us_cnt) printf("\n");
    }
    us_cnt = 0;
  }
  
}
