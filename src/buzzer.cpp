#include "buzzer.h"
#include "config.h"

int melody[] = {0,262,330,392,523,0};
int idx = 0;
unsigned long t = 0;
bool done = false;

void buzzerInit() {
  pinMode(BUZZER_PIN, OUTPUT);
}

bool buzzerStartup() {

  if (done) return true;

  if (millis() - t > 150) {
    t = millis();

    if (idx < 6) {
      if (melody[idx] == 0) {
        noTone(BUZZER_PIN);
      } else {
        tone(BUZZER_PIN, melody[idx]);
      }
      idx++;
    } else {
      noTone(BUZZER_PIN);
      done = true;
    }
  }
  return done;
}

void buzzerAlert(float temp, float soil) {

  if (temp > 35.0 || soil < 20.0) {
    tone(BUZZER_PIN, 2000);
  } else {
    noTone(BUZZER_PIN);
  }
}
