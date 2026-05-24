#include <avr/sleep.h>
#include <avr/power.h>

#define J_VRX1 A0
#define J_VRY1 A1
#define J_SW1 A2

#define J_VRX2 A3
#define J_VRY2 A4
#define J_SW2 A5

#define SW1 2
#define SW2 3
#define SW3 4
#define SW4 5
#define SW5 9
#define SW6 8
#define SW7 7
#define SW8 6
#define SW9 13
#define SW10 12
#define SW11 11
#define SW12 10

const unsigned long SLEEP_TIMEOUT_MS = 15000;
const int CHANGE_THRESHOLD = 4;

volatile bool wakeFlag = false;

int prevJS1X = 999, prevJS1Y = 999, prevJS1SW = -1;
int prevJS2X = 999, prevJS2Y = 999, prevJS2SW = -1;
int prevSW[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

unsigned long lastActivity = 0;

void wakeISR() {
  wakeFlag = true;
}

void sendState(int js1X, int js1Y, int js1SW, int js2X, int js2Y, int js2SW, int sw[12]) {
  Serial1.print("JS1X="); Serial1.print(js1X);
  Serial1.print(" JS1Y="); Serial1.print(js1Y);
  Serial1.print(" JS1SW="); Serial1.print(js1SW);

  Serial1.print(" JS2X="); Serial1.print(js2X);
  Serial1.print(" JS2Y="); Serial1.print(js2Y);
  Serial1.print(" JS2SW="); Serial1.print(js2SW);

  for (int i = 0; i < 12; i++) {
    Serial1.print(" SW");
    Serial1.print(i + 1);
    Serial1.print("=");
    Serial1.print(sw[i]);
  }
  Serial1.println();
}

void readInputs(int &js1X, int &js1Y, int &js1SW, int &js2X, int &js2Y, int &js2SW, int sw[12]) {
  js1X = map(analogRead(J_VRX1), 0, 1023, -100, 100);
  js1Y = map(analogRead(J_VRY1), 0, 1023, -100, 100);
  js1SW = digitalRead(J_SW1);

  js2X = map(analogRead(J_VRX2), 0, 1023, -100, 100);
  js2Y = map(analogRead(J_VRY2), 0, 1023, -100, 100);
  js2SW = digitalRead(J_SW2);

  sw[0] = digitalRead(SW1);
  sw[1] = digitalRead(SW2);
  sw[2] = digitalRead(SW3);
  sw[3] = digitalRead(SW4);
  sw[4] = digitalRead(SW5);
  sw[5] = digitalRead(SW6);
  sw[6] = digitalRead(SW7);
  sw[7] = digitalRead(SW8);
  sw[8] = digitalRead(SW9);
  sw[9] = digitalRead(SW10);
  sw[10] = digitalRead(SW11);
  sw[11] = digitalRead(SW12);
}

bool changedEnough(int js1X, int js1Y, int js1SW, int js2X, int js2Y, int js2SW, int sw[12]) {
  if (abs(js1X - prevJS1X) > CHANGE_THRESHOLD || abs(js1Y - prevJS1Y) > CHANGE_THRESHOLD || js1SW != prevJS1SW) return true;
  if (abs(js2X - prevJS2X) > CHANGE_THRESHOLD || abs(js2Y - prevJS2Y) > CHANGE_THRESHOLD || js2SW != prevJS2SW) return true;

  for (int i = 0; i < 12; i++) {
    if (sw[i] != prevSW[i]) return true;
  }
  return false;
}

void updatePrev(int js1X, int js1Y, int js1SW, int js2X, int js2Y, int js2SW, int sw[12]) {
  prevJS1X = js1X; prevJS1Y = js1Y; prevJS1SW = js1SW;
  prevJS2X = js2X; prevJS2Y = js2Y; prevJS2SW = js2SW;
  for (int i = 0; i < 12; i++) prevSW[i] = sw[i];
}

void goToSleep() {
  Serial1.flush();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  attachInterrupt(digitalPinToInterrupt(SW1), wakeISR, LOW);
  attachInterrupt(digitalPinToInterrupt(SW2), wakeISR, LOW);
  attachInterrupt(digitalPinToInterrupt(SW3), wakeISR, LOW);
  attachInterrupt(digitalPinToInterrupt(SW4), wakeISR, LOW);
  attachInterrupt(digitalPinToInterrupt(SW5), wakeISR, LOW);
  attachInterrupt(digitalPinToInterrupt(SW6), wakeISR, LOW);
  attachInterrupt(digitalPinToInterrupt(SW7), wakeISR, LOW);
  attachInterrupt(digitalPinToInterrupt(SW8), wakeISR, LOW);

  sleep_cpu();

  sleep_disable();

  detachInterrupt(digitalPinToInterrupt(SW1));
  detachInterrupt(digitalPinToInterrupt(SW2));
  detachInterrupt(digitalPinToInterrupt(SW3));
  detachInterrupt(digitalPinToInterrupt(SW4));
  detachInterrupt(digitalPinToInterrupt(SW5));
  detachInterrupt(digitalPinToInterrupt(SW6));
  detachInterrupt(digitalPinToInterrupt(SW7));
  detachInterrupt(digitalPinToInterrupt(SW8));
}

void setup() {
  Serial1.begin(115200);

  pinMode(J_SW1, INPUT_PULLUP);
  pinMode(J_SW2, INPUT_PULLUP);

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(SW5, INPUT_PULLUP);
  pinMode(SW6, INPUT_PULLUP);
  pinMode(SW7, INPUT_PULLUP);
  pinMode(SW8, INPUT_PULLUP);
  pinMode(SW9, INPUT_PULLUP);
  pinMode(SW10, INPUT_PULLUP);
  pinMode(SW11, INPUT_PULLUP);
  pinMode(SW12, INPUT_PULLUP);

  int js1X, js1Y, js1SW, js2X, js2Y, js2SW;
  int sw[12];
  readInputs(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);
  updatePrev(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);
  sendState(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);

  lastActivity = millis();
}

void loop() {
  int js1X, js1Y, js1SW, js2X, js2Y, js2SW;
  int sw[12];

  readInputs(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);

  bool changed = changedEnough(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);

  if (changed) {
    sendState(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);
    updatePrev(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);
    lastActivity = millis();
  }

  if (millis() - lastActivity > SLEEP_TIMEOUT_MS) {
    goToSleep();
    wakeFlag = false;
    lastActivity = millis();
    readInputs(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);
    sendState(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);
    updatePrev(js1X, js1Y, js1SW, js2X, js2Y, js2SW, sw);
  }

  delay(10);
}
