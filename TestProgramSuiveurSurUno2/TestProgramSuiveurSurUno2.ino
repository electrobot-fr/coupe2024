// Include Adafruit PWM Library
#include <Servo.h>


// les detecteurs infrarouge envoi une valeur aux alentours de 1000 quand ils ne détectent rien, proche de 0 sinon.
// on utilise la fonction millis() et non les delay() qui bloquent l'execution du code


#define MIN_DETECTION 400
#define POSITION_SERVO_REPOS1 65
#define POSITION_SERVO_REPOS2 65
#define ANGLE_POUSSE 30

int detecteurPin1[2] = { A0, A1 };
int detecteurPin2[2] = { A2, A3 };
bool panneauDetecte1[2] = { false, false };
bool panneauDetecte2[2] = { false, false };

bool panneauDetecte_Old1[2] = { false, false };
bool panneauDetecte_Old2[2] = { false, false };
int servoPin[2] = { 6, 5 };

int angleServo[2] = { POSITION_SERVO_REPOS1, POSITION_SERVO_REPOS2 };
unsigned long dateDetection[2] = { 0, 0 };
unsigned long dateCourante[2] = { 0, 0 };
const unsigned long dureeAvantCoupPanneau = 200;
Servo monServo1;
Servo monServo2;



void setup() {
  pinMode(8, INPUT_PULLUP);

  monServo1.write(POSITION_SERVO_REPOS1);
  monServo1.attach(servoPin[0]);
  monServo2.write(POSITION_SERVO_REPOS2);
  monServo2.attach(servoPin[1]);

  PousserPanneau1(POSITION_SERVO_REPOS1);
  PousserPanneau2(POSITION_SERVO_REPOS2);

  Serial.begin(9600);
  Serial.println("c'est parti");
  dateCourante[0] = millis();
  dateCourante[1] = millis();

  dateDetection[0] = millis();
  dateDetection[1] = millis();


  PousserPanneau1(POSITION_SERVO_REPOS1);
  PousserPanneau2(POSITION_SERVO_REPOS2);
}

byte Detecter(int pinDetection) {
  if (analogRead(pinDetection) > MIN_DETECTION) {
    return true;
  } else {
    return false;
  }
}

void PousserPanneau1(int angle) {
  monServo1.write(angle);
  delay(500);
  monServo1.write(POSITION_SERVO_REPOS1);
  angleServo[0] = POSITION_SERVO_REPOS1;
}
void PousserPanneau2(int angle) {
  monServo2.write(angle);
  delay(500);
  monServo2.write(POSITION_SERVO_REPOS2);
  angleServo[1] = POSITION_SERVO_REPOS2;
}

bool up = false;

void loop() {
  if (digitalRead(8) == LOW) {
    monServo1.write(POSITION_SERVO_REPOS1 - ANGLE_POUSSE);
    monServo2.write(POSITION_SERVO_REPOS2 + ANGLE_POUSSE);
    up = true;
    return;
  }
  if (up == true) {
    monServo1.write(POSITION_SERVO_REPOS1);
    monServo2.write(POSITION_SERVO_REPOS2);
    up = false;
  }

  // Boucle de detection de sortie d'un panneau par la droite ou la gauche
  for (byte i = 0; i < 2; i++) {
    panneauDetecte1[i] = Detecter(detecteurPin1[i]);
    panneauDetecte2[i] = Detecter(detecteurPin2[i]);
  }


  if (((panneauDetecte1[0] == false) && (panneauDetecte_Old1[0] == true)) && (panneauDetecte1[1] == false)) {
    dateDetection[0] = millis();
    angleServo[0] = POSITION_SERVO_REPOS1 + ANGLE_POUSSE;
    Serial.print("detecte vers gauche");
    Serial.println(angleServo[0]);
  } else if (((panneauDetecte1[1] == false) && (panneauDetecte_Old1[1] == true)) && (panneauDetecte1[0] == false)) {
    dateDetection[0] = millis();
    angleServo[0] = POSITION_SERVO_REPOS1 - ANGLE_POUSSE;
    Serial.print("detecte vers droite");
    Serial.println(angleServo[0]);
  }


  if (((panneauDetecte2[0] == false) && (panneauDetecte_Old2[0] == true)) && (panneauDetecte2[1] == false)) {
    dateDetection[1] = millis();
    angleServo[1] = POSITION_SERVO_REPOS2 + ANGLE_POUSSE;
    Serial.print("detecte vers gauche");
    Serial.println(angleServo[1]);
  } else if (((panneauDetecte2[1] == false) && (panneauDetecte_Old2[1] == true)) && (panneauDetecte2[0] == false)) {
    dateDetection[1] = millis();
    angleServo[1] = POSITION_SERVO_REPOS1 - ANGLE_POUSSE;
    Serial.print("detecte vers droite");
    Serial.println(angleServo[1]);
  }


  dateCourante[0] = millis();
  panneauDetecte_Old1[0] = panneauDetecte1[0];
  panneauDetecte_Old1[1] = panneauDetecte1[1];

  dateCourante[1] = millis();
  panneauDetecte_Old2[0] = panneauDetecte2[0];
  panneauDetecte_Old2[1] = panneauDetecte2[1];


  if ((dateCourante[0] > (dateDetection[0] + dureeAvantCoupPanneau)) && (angleServo[0] != POSITION_SERVO_REPOS1)) {
    Serial.print("je pousse");
    Serial.println(angleServo[0]);
    PousserPanneau1(angleServo[0]);
  }

  if ((dateCourante[1] > (dateDetection[1] + dureeAvantCoupPanneau)) && (angleServo[1] != POSITION_SERVO_REPOS2)) {
    Serial.print("je pousse");
    Serial.println(angleServo[1]);
    PousserPanneau2(angleServo[1]);
  }
}