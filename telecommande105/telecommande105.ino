#include "SerialTransfer.h"
#include <TM1637Display.h>

TM1637Display display(11, 10);
SerialTransfer transfer;

struct __attribute__((packed)) STRUCT {
  int16_t x;
  int16_t y;
  bool a;
  bool b;

  uint8_t asc1;
  uint8_t asc2;
  uint8_t pince1;
  uint8_t pince2;
  uint8_t compteur;

  bool poussePanneau;
} message;

void setup() {
  // Serial.begin(9600);
  Serial.begin(115200);
  transfer.begin(Serial);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  // poussePanneau
  pinMode(6, INPUT_PULLUP);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  message.compteur = 25;

  display.setBrightness(0x0F);
}

int plusUn = 0;
int moinsUn = 0;
int compteur = 0;

void loop() {
  if (digitalRead(3) == 0 && plusUn == 1) {
    message.compteur = message.compteur + 1;
  }
  if (digitalRead(2) == 0 && moinsUn == 1) {
    message.compteur = message.compteur - 1;
  }
  plusUn = digitalRead(3);
  moinsUn = digitalRead(2);


  message.x = analogRead(A0);
  message.y = analogRead(A1);
  message.b = digitalRead(8) == LOW;
  message.a = digitalRead(9) == LOW;
  message.poussePanneau = digitalRead(6) == LOW;

  if (digitalRead(4) == 0) {
    message.pince1 = 130; // 130
    message.pince2 = 130; 
  } else {
    message.pince1 = 55; // 60
    message.pince2 = 60;
  }
  if (digitalRead(5) == 1) {
    message.asc1 = 90 - 25; // bas
    message.asc2 = 70 + 25;
  } else {
    message.asc1 = 90 + 25; // haut
    message.asc2 = 70 - 25;
  }


  uint16_t sendSize = 0;
  sendSize = transfer.txObj(message, sendSize);
  transfer.sendData(sendSize);

  if (compteur != message.compteur) {
    display.showNumberDecEx(message.compteur);
    compteur = message.compteur;
  }

  delay(10);
}
