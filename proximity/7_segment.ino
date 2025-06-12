#include <TM1638plus.h>

#define STB 4
#define CLK 3
#define DIO 2
#define TRIG_PIN 9
#define ECHO_PIN 10

TM1638plus display(STB, CLK, DIO);

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  display.displayBegin();
  display.reset();

  // Show 0000 at start
  display.displayText("0000    ");  // 8 chars needed, pad with spaces
  delay(1000);
}

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH);
}

void loop() {
  long total = 0;
  for (int i = 0; i < 5; i++) {
    total += readDistance();
    delay(50);
  }

  long duration = total / 5;
  float cm = duration * 0.034 / 2.0;
  int shown = (int)(cm + 0.5);

  if (shown < 2 || shown > 400) {
    shown = 0;
  }

  // Create zero-padded 4-digit string
  char buffer[9];  // 8 chars + null terminator
  snprintf(buffer, sizeof(buffer), "%04d    ", shown); // Pad to 8 chars total (4 digits + 4 spaces)

  display.displayText(buffer);

  delay(250);
}
