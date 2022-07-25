/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 13
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 


long threshTime = 0;
long time = 0;
//chân ST_CP của 74HC595
const byte clockPin = 12;
//chân SH_CP của 74HC595
const byte dataPin = 11;
//Chân DS của 74HC595
const byte latchPin = 8;
const byte neonSdt = 10;
const byte relay = 9;   
const byte neonNumber1 = 3;
const byte neonXaHang = 6;
const byte neonGiaDo = 5;
const byte button3 = 2; // chuyển chế độ nháy
const byte button2 = 7; // chuyển giá tiền
const byte button1 = 4; // bật tắt neon xả hàng
short point = 0;
short tempt = 0;
short blinkMode = 0;
short leds = 0;
bool checkRun = 0;
bool statusNeonNumber1 = 0;
bool neonXaHangStatus = 0;
bool buttonStatus1 = 0;
bool lastStatus1 = 1;
bool buttonStatus2 = 0;
bool buttonStatus3 = 0;
bool lastStatus3 = 1;

//mảng có 10 số (từ 0-9) và 
const int Seg[20] = {
  0b10111111,//0 - các thanh từ f-a sáng
  0b10000110,//1 - chỉ có 2 thanh b,c sáng
  0b11011011,//2 - thứ tự g f e d c b a
  0b11001111,//3
  0b11100110,//4
  0b11101101,//5
  0b11111101,//6
  0b10000111,//7
  0b11111111,//8
  0b11101111,//9
  0b10111111,//0 - các thanh từ f-a sáng
  0b10000110,//1 - chỉ có 2 thanh b,c sáng
  0b11011011,//2 - thứ tự g f e d c b a
  0b11001111,//3
  0b11100110,//4
  0b11101101,//5
  0b11111101,//6
  0b10000111,//7
  0b11111111,//8
  0b11101111,//9
};


void setup() {
  //Bạn BUỘC PHẢI pinMode các chân này là OUTPUT
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(neonGiaDo, OUTPUT);
  pinMode(neonXaHang, OUTPUT);
  pinMode(neonNumber1, OUTPUT);
  pinMode(neonSdt, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(relay, OUTPUT );
  sensors.begin(); 
  Serial.begin(9600);
}



void change_price(){
      if(millis() - time > 1000){
        point = (point + 1) % 20; // Vòng tuần hoàn từ 0--19
        if(point > 9){
        Serial.println(point);
        statusNeonNumber1 = HIGH;
        }else{
        statusNeonNumber1 = LOW;
        }
        digitalWrite(neonNumber1, statusNeonNumber1);
        time = millis();
      }
    
}

void change_blink_mode(){
switch(blinkMode){
  static int i;
  case 0:
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
    delay(500);
    digitalWrite(neonGiaDo, HIGH);
    delay(500);
    digitalWrite(neonXaHang, HIGH);
    delay(500);
    digitalWrite(neonNumber1, HIGH);
    delay(500);
    for(i = 0; i < 8; ++i){
      digitalWrite(latchPin, LOW);
      bitSet(leds, i);
      shiftOut(dataPin, clockPin, MSBFIRST, leds);
      digitalWrite(latchPin, HIGH);
      delay(500); 
    }
    digitalWrite(neonSdt, HIGH);
    delay(1000); 

    for(i = 0; i < 3; ++i){
      digitalWrite(neonGiaDo, LOW);
      digitalWrite(neonXaHang, LOW);
      digitalWrite(neonNumber1, LOW);
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0);
      digitalWrite(latchPin, HIGH);
      digitalWrite(neonSdt, LOW);
      delay(500);
      digitalWrite(neonGiaDo, HIGH);
      digitalWrite(neonXaHang, HIGH);
      digitalWrite(neonNumber1, HIGH);
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
      digitalWrite(latchPin, HIGH);
      digitalWrite(neonSdt, HIGH);
      delay(500);
    }
    blinkMode = -1;
    break;

  case 1:
    for(i = 0; i < 2; ++i){
      digitalWrite(neonGiaDo, LOW);
      delay(500);
      digitalWrite(neonGiaDo, HIGH);
      delay(500);
      }
      if(neonXaHangStatus == HIGH){
        for(i = 0; i < 2; ++i){
          digitalWrite(neonXaHang, LOW);
          delay(500);
          digitalWrite(neonXaHang, HIGH);
          delay(500);
        }
      }
    
      for(i = 0; i < 5; ++i){
      digitalWrite(neonGiaDo, HIGH);
      digitalWrite(neonXaHang, LOW);
      delay(500);
      if(neonXaHangStatus == HIGH)
      digitalWrite(neonXaHang, HIGH);
      digitalWrite(neonGiaDo, LOW);
      delay(500);
      digitalWrite(neonGiaDo, HIGH);
      if(neonXaHangStatus == HIGH)
      digitalWrite(neonXaHang, neonXaHangStatus);
      }

      delay(500);
      for(i = 0; i < 5; ++i){
      if(point > 9) digitalWrite(neonNumber1, LOW);
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0); 
      digitalWrite(latchPin, HIGH);
      delay(500);
      if(point > 9) digitalWrite(neonNumber1, HIGH);
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, Seg[point]); 
      digitalWrite(latchPin, HIGH);
      delay(500);
      }
    threshTime = millis();
    checkRun = 1;    
    break;

  case 2: 
    digitalWrite(neonGiaDo, LOW);
    digitalWrite(neonXaHang, LOW);
    digitalWrite(neonNumber1, LOW);
    digitalWrite(neonSdt, LOW);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
    delay(500);
    for(i = 0; i < 255; i += 5){
       analogWrite(neonGiaDo, i);
       delay(20);
    }
    for(i = 0; i < 3; ++i){
      digitalWrite(neonGiaDo, LOW);
      delay(200);
      digitalWrite(neonGiaDo, HIGH);
      delay(200);
    }
    delay(300);
    if(neonXaHangStatus == HIGH){
      for(i = 0; i < 255; i += 5){
       analogWrite(neonXaHang, i);
       delay(20);
      }
      for(i = 0; i < 3; ++i){
        digitalWrite(neonXaHang, LOW);
        delay(200);
        digitalWrite(neonXaHang, HIGH);
        delay(200);
      }
      delay(300);
    }
      
    
    for(i = 0; i < 4; ++i){
      if(point > 9) digitalWrite(neonNumber1, LOW);
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0); 
      digitalWrite(latchPin, HIGH);
      delay(200);
      if(point > 9) digitalWrite(neonNumber1, HIGH);
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, Seg[point]); 
      digitalWrite(latchPin, HIGH);
      delay(200);
      }

      for(i = 0; i < 255; i += 5){
       analogWrite(neonSdt, i);
       delay(30);
      }
    for(i = 0; i < 3; ++i){
      digitalWrite(neonSdt, LOW);
      delay(200);
      digitalWrite(neonSdt, HIGH);
      delay(200);
    }
    delay(300);
    
      for(int j = 0; j < 5; ++j){
        for(i = 255; i >= 0; i -= 5){
          analogWrite(neonGiaDo, i);
          if(neonXaHangStatus == HIGH){
            analogWrite(neonXaHang, i);
          }
          analogWrite(neonSdt, i);
          delay(20);
        }
        delay(200);
        for(i = 0; i <= 255; i += 5){
          analogWrite(neonGiaDo, i);
          if(neonXaHangStatus == HIGH){
            analogWrite(neonXaHang, i);
          }
          analogWrite(neonSdt, i);
          delay(20);
        }
        delay(200);
      }
      threshTime = millis();
      checkRun = 1;   
    break;
  default: 
    digitalWrite(neonGiaDo, HIGH);
    digitalWrite(neonXaHang, neonXaHangStatus);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, Seg[point]); 
    digitalWrite(latchPin, HIGH);
    if(point > 9){
        statusNeonNumber1 = HIGH;
        }else{
        statusNeonNumber1 = LOW;
        }
    digitalWrite(neonNumber1, statusNeonNumber1); 
    digitalWrite(neonSdt, HIGH); 
    break;  
  }
}

void reduce_heat(){
  sensors.requestTemperatures(); 
  if(sensors.getTempCByIndex(0) >= 39){
    
    digitalWrite(relay, HIGH);
  }else{
    digitalWrite(relay, LOW);
  }
  Serial.println(sensors.getTempCByIndex(0));
}


void loop() {
  if(checkRun != 1 || millis() - threshTime > 5000 || blinkMode == -1 || blinkMode == 0)
    change_blink_mode();

  buttonStatus1 = digitalRead(button1);
  if(buttonStatus1 != lastStatus1){
    neonXaHangStatus = !neonXaHangStatus;
    digitalWrite(neonXaHang, neonXaHangStatus);
    lastStatus1 = buttonStatus1;
  } 


  
  buttonStatus3 = digitalRead(button3);
  if(buttonStatus3 != lastStatus3){
    if(buttonStatus3 == HIGH){
     blinkMode = -1;
     checkRun = 0;
    }
    else{
     tempt++;
     if(tempt > 2) tempt = 1;
     blinkMode = tempt;  
    } 
    lastStatus3 = buttonStatus3;
    delay(20); 
  }
    
  buttonStatus2 = digitalRead(button2);
    if(buttonStatus2 == LOW){
      change_price();
      digitalWrite(latchPin, LOW);
    //Xuất bảng ký tự ra cho Module LED
      shiftOut(dataPin, clockPin, MSBFIRST, Seg[point]); 
      digitalWrite(latchPin, HIGH);
    //shiftout - end
    }else{
      time = millis();
    }
    //shiftout - start
  
  reduce_heat();
  
}