/*
  Первая часть скетча для первой Ардуины,
  которая присоеденяется к перчатке, которая
  управляет бионическую руку,
  напечатанную на 3Д принтере, с помощю
  Arduino Nano, серво приводов,
  расширителя аналоговых пинов на базе
  здвигового регистра CD74HC4067, потонциометров
  и шилда для серв PCA9685

  By Dairis Mivreniks 2020
*/


//******************************** НАСТРОЙКИ *********************************************

// Пины подключение аналогового мултиплексора
#define S0 7
#define S1 6
#define S2 5
#define S3 4
#define SIG A0

#define BUTTON 3
#define LED 8


#define SERVOMIN  145 // Минимальное время импульса
#define SERVOMAX  515 // Максимальное время импульса


//***************************** ДЛЯ РАЗРАБОТЧИКОВ *****************************

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio (9, 10); // создать модуль на пинах 9 и 10 для Нано и Уно

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

int cont[17];
int setPot[17][2];

byte gotByte;

boolean butt;




void setup() {

  Serial.begin(9600);

  radio.begin();

  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_2MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  // ВНИМАНИЕ!!! enableAckPayload НЕ РАБОТАЕТ НА СКОРОСТИ 250 kbps!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);


  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  digitalWrite(LED, false);

}


void loop() {

  butt = !digitalRead(BUTTON);


  if (butt == 1) {

    delay(3000);

    for (byte val = 0; val < 16; val++) {
      setPot[val][0] = readMux(val);
    }

    digitalWrite(LED, true);
    delay(3000);

    for (byte hi = 0; hi < 16; hi++) {
      setPot[hi][1] = readMux(hi);
    }

    digitalWrite(LED, false);
  }


  for (int i = 0; i < 16; i ++) {
    cont[i] = readMux(i);

    radio.write(&cont, sizeof(cont));

  }
}


int readMux(int channel) {
  int controlPin[] = {S0, S1, S2, S3};

  int muxChannel[16][4] = {
    {0, 0, 0, 0}, //channel 0
    {1, 0, 0, 0}, //channel 1
    {0, 1, 0, 0}, //channel 2
    {1, 1, 0, 0}, //channel 3
    {0, 0, 1, 0}, //channel 4
    {1, 0, 1, 0}, //channel 5
    {0, 1, 1, 0}, //channel 6
    {1, 1, 1, 0}, //channel 7
    {0, 0, 0, 1}, //channel 8
    {1, 0, 0, 1}, //channel 9
    {0, 1, 0, 1}, //channel 10
    {1, 1, 0, 1}, //channel 11
    {0, 0, 1, 1}, //channel 12
    {1, 0, 1, 1}, //channel 13
    {0, 1, 1, 1}, //channel 14
    {1, 1, 1, 1} //channel 15
  };

  //loop through the 4 sig
  for (int i = 0; i < 4; i ++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  // читать значение с аналогого пина SIG
  int val = analogRead(SIG);
  val = map(val, setPot[channel][1], setPot[channel][0], SERVOMIN, SERVOMAX);
  val = constrain(val, SERVOMIN, SERVOMAX);
  //val = map(val, 0, 1023, SERVOMIN, SERVOMAX); // Меняем диапазон для работы с сервами

  // возращаем значение val
  return val;
}
