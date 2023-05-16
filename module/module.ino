//#define DEBUG

#ifdef DEBUG
#include "DigiKeyboard.h"
#endif

#define SHUTDOWN_MONITOR_PIN 2
#define SHUTDOWN_EXECUTE_PIN 0
#define LED_PIN 1
#define STARTUP_DELAY 7000          // Time interval to allow the printer boot up, ms
#define STANDBY_BLINK_PERIOD 2000   // Time period for the LED signaling the STANDBY state, ms
#define SHUTDOWN_BLINK_PERIOD 500   // Time period for the LED signaling the SHUTDOWN_EXECUTED state, ms
#define FILTER_COUNT 3              // Number of additional repeated readings to confirm a shutdown request
#define FILTER_PERIOD 500           // Time interval between the repeated readings, ms

bool isShutdownRequested = false;
bool isShutdownExecuted = false;
bool ledState = false;
unsigned long lastMillis;           // Timer variable

// ------------------------------------ Subroutines ------------------------------------

// Monitor the shutdown request from the printer
void monitorShutdownRequest() {
#ifdef DEBUG
    DigiKeyboard.println("monitorShutdownRequest() called");
#endif
    if (digitalRead(SHUTDOWN_MONITOR_PIN) == LOW) isShutdownRequested = true;
}


// Perform the printer shutdown
void doShutdown() {
#ifdef DEBUG
  DigiKeyboard.println("doShutdown() called");
#endif

  // Filter the event
  for (byte i = 0; i < FILTER_COUNT; i++) {
    delay(FILTER_PERIOD);
    if (digitalRead(SHUTDOWN_MONITOR_PIN) == HIGH) {    // Repeat the reading and check if it is consistent
      isShutdownRequested = false;                      // False positive, reset the flag and exit
      return;
    }
  }                                                     // All the filter readings are consistent, preceed to shutdown
  digitalWrite(SHUTDOWN_EXECUTE_PIN, LOW);
  isShutdownExecuted = true;
}


// Invert the LED
void invertLed() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}


// Blink the LED indicating the STANDBY state
void blinkLedForStandby() {
  if (millis() - lastMillis >= STANDBY_BLINK_PERIOD) {
    lastMillis = millis();
    invertLed();
  }
}


// Blink the LED indicating the SHUTDOWN_EXECUTED state
void blinkLedForShutdownExecuted() {
  if (millis() - lastMillis >= SHUTDOWN_BLINK_PERIOD) {
    lastMillis = millis();
    invertLed();
  }
}


// ------------------------------------ Core routines ------------------------------------

// Arduino setup
void setup() {
  pinMode(SHUTDOWN_MONITOR_PIN, INPUT_PULLUP);
  pinMode(SHUTDOWN_EXECUTE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(SHUTDOWN_EXECUTE_PIN, HIGH);   // Standby; the shutdown relay activates on LOW
  digitalWrite(LED_PIN, LOW);                 // Turn off the LED

  delay(STARTUP_DELAY);
}


// Main loop
void loop() {

#ifdef DEBUG
  DigiKeyboard.println("loop() iterated");
  delay(2000);
#endif

  if (!isShutdownRequested) {                         // STANDBY state
    monitorShutdownRequest();
    blinkLedForStandby();
  } 
  if (isShutdownRequested && !isShutdownExecuted) {   // STUTDOWN_REQUESTED state
    doShutdown();
  }
  if (isShutdownExecuted) {                           // SHUTDOWN_EXECUTED state
    blinkLedForShutdownExecuted();
  }
  
}


