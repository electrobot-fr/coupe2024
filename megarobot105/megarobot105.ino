#define pinX A12
#define pinY A11
#include <Servo.h>
#include "SerialTransfer.h"
#include <TM1637Display.h>

SerialTransfer transfer;
TM1637Display display(20, 21);

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

Servo asc1;
Servo asc2;
Servo pince3;
Servo pince2;
Servo pince1;

int motor1Pin1 = 47;
int motor1Pin2 = 45;
int enablePin1 = 8;

int motor2Pin1 = 52;
int motor2Pin2 = 53;
int enablePin2 = 10;

int motor4Pin1 = 37;
int motor4Pin2 = 39;
int enablePin4 = 4;

int motor3Pin1 = 43;
int motor3Pin2 = 49;
int enablePin3 = 11;

// Vitesse du moteur
int state = 0;
void setup() {
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);

  pince3.attach(2);
  pince3.write(120);

  pince2.attach(3);
  pince2.write(120);

  pince1.attach(9);
  pince1.write(120);

  asc1.attach(5);
  asc1.write(80);
  
  asc2.attach(7);
  asc2.write(85);

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  // Configuration des ports en mode "sortie"
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enablePin1, OUTPUT);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enablePin2, OUTPUT);

  pinMode(motor3Pin1, OUTPUT);
  pinMode(motor3Pin2, OUTPUT);
  pinMode(enablePin3, OUTPUT);

  pinMode(motor4Pin1, OUTPUT);
  pinMode(motor4Pin2, OUTPUT);
  pinMode(enablePin4, OUTPUT);

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  // Initialisation du port série
  Serial.begin(115200);
  Serial.setTimeout(100);

  Serial2.begin(115200);
  transfer.begin(Serial2);

  display.setBrightness(0x0F);
  display.showNumberDecEx(0);
}

int maxV = 100;
int maxV_Translation = 80;

void loop() {
  if (transfer.available()) {
    uint16_t recSize = 0;
    recSize = transfer.rxObj(message, recSize);

    // Serial.print("asc 1 = ");
    // Serial.print(message.asc1);
    // Serial.print(", asc 2 = ");
    // Serial.print(message.asc2);
    // Serial.print("pince 1 = ");
    // Serial.print(message.pince1);
    // Serial.print(", pince 2 = ");
    // Serial.println(message.pince2);

    display.showNumberDecEx(message.compteur);

    int X = message.x;
    int Y = message.y;

    int Xs = map(X, 0, 1023, -maxV, maxV);
    int Ys = map(Y, 0, 1023, -maxV, maxV);

    bool boutonA = message.a;
    bool boutonB = message.b;
    
    if (boutonA) {
      state = maxV_Translation;
      tourneragauche();
    } else if (boutonB) {
      state = maxV_Translation;
      tourneradroite();

    } else if (Xs < -35 || Xs > 35) {
      state = Xs;
      if (Xs > 0) {
        arriere();
      } else {
        avant();
      }
    } else if (Ys < -35 || Ys > 35) {
      state = Ys;
      if (Ys > 0) {
      gauche();
      } else {
      droite();
      }
    } else {
      state = 0;
      stop();
    }
    analogWrite(enablePin1, abs(state));
    analogWrite(enablePin2, abs(state));
    analogWrite(enablePin3, abs(state));
    analogWrite(enablePin4, abs(state));

    pince3.write(message.pince2);
    pince2.write(message.pince2);
    pince1.write(message.pince1);
    asc1.write(message.asc1);
    asc2.write(message.asc2);

    if (message.poussePanneau) {
      digitalWrite(15, LOW);
    } else {
      digitalWrite(15, HIGH);
    }
  }

  if (Serial.available()) {
    int input = Serial.parseInt();
    if (input == 0) {
      return;
    }
    Serial.println(input);
    if (input == 1) {
      pince2.write(150);
      pince1.write(150);
      return;
    }
    if (input == 2) {
      pince2.write(70);
      pince1.write(70);
      return;
    }
    
    asc1.write(80 + input);
    asc2.write(85 - input);
  }

  // delay(10);
  // return;
}

void avant() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  digitalWrite(motor3Pin1, HIGH);
  digitalWrite(motor3Pin2, LOW);

  digitalWrite(motor4Pin1, HIGH);
  digitalWrite(motor4Pin2, LOW);

  // Serial.print("Avant ");
  // Serial.println(state);
}

void arriere() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);

  digitalWrite(motor3Pin1, LOW);
  digitalWrite(motor3Pin2, HIGH);

  digitalWrite(motor4Pin1, LOW);
  digitalWrite(motor4Pin2, HIGH);

  // Serial.print("Arriere ");
  // Serial.println(state);
}

void droite() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  digitalWrite(motor3Pin1, HIGH);
  digitalWrite(motor3Pin2, LOW);

  digitalWrite(motor4Pin1, LOW);
  digitalWrite(motor4Pin2, HIGH);

  // Serial.print("droite ");
  // Serial.println(state);
}

void stop() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, HIGH);

  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, HIGH);

  digitalWrite(motor3Pin1, HIGH);
  digitalWrite(motor3Pin2, HIGH);

  digitalWrite(motor4Pin1, HIGH);
  digitalWrite(motor4Pin2, HIGH);

  // Serial.println("Stop");
}

void gauche() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);

  digitalWrite(motor3Pin1, LOW);
  digitalWrite(motor3Pin2, HIGH);

  digitalWrite(motor4Pin1, HIGH);
  digitalWrite(motor4Pin2, LOW);

  // Serial.print("gauche ");
  // Serial.println(state);
}

void tourneragauche() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);

  digitalWrite(motor3Pin1, HIGH);
  digitalWrite(motor3Pin2, LOW);

  digitalWrite(motor4Pin1, LOW);
  digitalWrite(motor4Pin2, HIGH);

  // Serial.print("tourneragauche ");
  // Serial.println(state);
}

void tourneradroite() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  digitalWrite(motor3Pin1, LOW);
  digitalWrite(motor3Pin2, HIGH);

  digitalWrite(motor4Pin1, HIGH);
  digitalWrite(motor4Pin2, LOW);

  // Serial.print("tourneradroite ");
  // Serial.println(state);
}
