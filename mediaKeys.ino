#include <HID-Project.h>

const uint8_t keyboardEnable = 21;
const uint8_t button[] = { 20, 19, 18, 15, 14 };
ConsumerKeycode keys[] = { MEDIA_VOL_DOWN, MEDIA_VOL_UP, MEDIA_PREV, MEDIA_PLAY_PAUSE, MEDIA_NEXT };

void setup() {
  // put your setup code here, to run once:
  for (const uint8_t b : button) {
    pinMode(b, INPUT_PULLUP);
  }
  pinMode(keyboardEnable, INPUT_PULLUP);

  Serial.begin(9600);
  Consumer.begin();
}

void ReadButtonStates(bool* states) {
  for (size_t i = 0; i < 5; i++) {
    states[i] = !digitalRead(button[i]);
  }
}

void DebounceStates(bool* states, bool* debouncedStates)
{
  static bool prevStates[5];
  static unsigned long tsDebounce = millis();
  
  if (millis() - tsDebounce < 5) return;

  tsDebounce = millis();
  for (size_t i = 0; i < 5; i++) {
    prevStates[i] = states[i];
    debouncedStates[i] = states[i] == prevStates[i] ? states[i] : false;
  }
}

void SendKeys(bool* states)
{
  static bool prevStates[5];
  for (size_t i = 0; i < 5; i++) {
    if (!prevStates[i] && states[i]) {
      Consumer.press(keys[i]);
      Serial.print("press "); Serial.println(keys[i]);
    }
    else if (prevStates[i] && !states[i]) {
      Consumer.release(keys[i]);
      Serial.print("release "); Serial.println(keys[i]);
    }
    prevStates[i] = states[i];
  }
}

void loop() {
  bool states[5];
  ReadButtonStates(states);
  static bool debouncedStates[5];
  DebounceStates(states, debouncedStates);

  // do nothing if disabled
  if (digitalRead(keyboardEnable)) return;

  SendKeys(debouncedStates);
}
