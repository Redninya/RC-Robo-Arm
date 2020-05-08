/*
  Вторая часть скетча для второй Ардуины,
  которая подключена к руке.

  By Dairis Mivreniks 2020
*/

//=================================== ДЛЯ РАЗРАБОТЧИКОВ ===============================

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб



void setup() {

  Serial.begin(9600);

  pwm.begin();
  pwm.setPWMFreq(50);
  radio.begin();


  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_2MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  // ВНИМАНИЕ!!! enableAckPayload НЕ РАБОТАЕТ НА СКОРОСТИ 250 kbps!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
}

void loop() {

  byte pipeNo;
  int pot;
  int cont[17];

  while (radio.available(&pipeNo)) {
    radio.read(&cont, sizeof(cont));
  }

  for (int i = 0; i < 16; i++) {
    pot = cont[i];
    pwm.setPWM(i, 0, pot);
  }
}
