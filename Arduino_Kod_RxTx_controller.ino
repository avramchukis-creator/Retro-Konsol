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

int prevJS1X = 999, prevJS1Y = 999, prevJS1SW = -1;
int prevJS2X = 999, prevJS2Y = 999, prevJS2SW = -1;
int prevSW[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

void setup() {
  Serial.begin(115200);

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
}

void loop() {
  int js1X = map(analogRead(J_VRX1), 0, 1023, -100, 100);
  int js1Y = map(analogRead(J_VRY1), 0, 1023, -100, 100);
  int js1SW = digitalRead(J_SW1);

  int js2X = map(analogRead(J_VRX2), 0, 1023, -100, 100);
  int js2Y = map(analogRead(J_VRY2), 0, 1023, -100, 100);
  int js2SW = digitalRead(J_SW2);

  int sw[12] = {
    digitalRead(SW1), digitalRead(SW2), digitalRead(SW3), digitalRead(SW4),
    digitalRead(SW5), digitalRead(SW6), digitalRead(SW7), digitalRead(SW8),
    digitalRead(SW9), digitalRead(SW10), digitalRead(SW11), digitalRead(SW12)
  };

  bool changed = false;
  if (abs(js1X - prevJS1X) > 2 || abs(js1Y - prevJS1Y) > 2 || js1SW != prevJS1SW) changed = true;
  if (abs(js2X - prevJS2X) > 2 || abs(js2Y - prevJS2Y) > 2 || js2SW != prevJS2SW) changed = true;

  for (int i = 0; i < 12; i++) {
    if (sw[i] != prevSW[i]) {
      changed = true;
      break;
    }
  }

  if (changed) {
    Serial.print("JS1X="); Serial.print(js1X);
    Serial.print(" JS1Y="); Serial.print(js1Y);
    Serial.print(" JS1SW="); Serial.print(js1SW);

    Serial.print(" JS2X="); Serial.print(js2X);
    Serial.print(" JS2Y="); Serial.print(js2Y);
    Serial.print(" JS2SW="); Serial.print(js2SW);

    for (int i = 0; i < 12; i++) {
      Serial.print(" SW");
      Serial.print(i + 1);
      Serial.print("=");
      Serial.print(sw[i]);
    }
    Serial.println();

    prevJS1X = js1X; prevJS1Y = js1Y; prevJS1SW = js1SW;
    prevJS2X = js2X; prevJS2Y = js2Y; prevJS2SW = js2SW;
    for (int i = 0; i < 12; i++) prevSW[i] = sw[i];
  }

  delay(5);
}