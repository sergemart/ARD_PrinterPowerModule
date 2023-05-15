//#define DEBUG

#ifdef DEBUG
#include "DigiKeyboard.h"
#endif

#define SHUTDOWN_MONITOR_PIN 2
#define SHUTDOWN_EXECUTE_PIN 0
#define STARTUP_DELAY 7000   // Time interval to allow the printer boot up

bool isShutdownRequested = false;
bool isShutdownExecuted = false;


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
  digitalWrite(SHUTDOWN_EXECUTE_PIN, LOW);
  isShutdownExecuted = true;
}


// ------------------------------------ Core routines ------------------------------------

// Arduino setup
void setup() {
  pinMode(SHUTDOWN_MONITOR_PIN, INPUT_PULLUP);
  pinMode(SHUTDOWN_EXECUTE_PIN, OUTPUT);

  digitalWrite(SHUTDOWN_EXECUTE_PIN, HIGH);   // standby; the shutdown relay is activated on LOW
  delay(STARTUP_DELAY);
}


// Main loop
void loop() {
#ifdef DEBUG
  DigiKeyboard.println("loop() iterated");
  delay(2000);
#endif
  if (!isShutdownRequested) monitorShutdownRequest();
  if (isShutdownRequested && !isShutdownExecuted) doShutdown();
}


