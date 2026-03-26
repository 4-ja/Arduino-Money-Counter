#include <Arduino.h>

// PIN DEFINITIONS
const int billPin = 2; // TB74 connected here (1 pulse = 10 PHP)
const int coinPin = 3; // Coin Slot connected here (1 pulse = 1 PHP)

// VOLATILE COUNTERS (Needed for Interrupts)
volatile int billPulses = 0;
volatile int coinPulses = 0;
unsigned long lastBillTime = 0;
unsigned long lastCoinTime = 0;

// GLOBAL BALANCE
int totalBalance = 0;

// INTERRUPT FOR BILLS (Digital Pin 2)
void billISR() {
  billPulses++;
  lastBillTime = millis();
}

// INTERRUPT FOR COINS (Digital Pin 3)
void coinISR() {
  coinPulses++;
  lastCoinTime = millis();
}

void setup() {
  // Matches your .ini monitor_speed = 115200
  Serial.begin(115200); 

  pinMode(billPin, INPUT_PULLUP);
  pinMode(coinPin, INPUT_PULLUP);

  // Set to FALLING since your hardware is active-low
  attachInterrupt(digitalPinToInterrupt(billPin), billISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(coinPin), coinISR, FALLING);

  Serial.println("\n=================================");
  Serial.println("   PANCAFE DUAL KIOSK: ONLINE   ");
  Serial.println("=================================");
}

void loop() {
  // PROCESS BILLS (Wait 500ms after last pulse to confirm amount)
  if (billPulses > 0 && (millis() - lastBillTime > 500)) {
    int amount = billPulses * 10; 
    totalBalance += amount;
    
    Serial.print("[BILL] Detected: "); Serial.print(billPulses);
    Serial.print(" pulses | Added: PHP "); Serial.println(amount);
    Serial.print(">>> NEW BALANCE: PHP "); Serial.println(totalBalance);
    
    billPulses = 0; // Reset for next bill
  }

  // PROCESS COINS (Wait 500ms after last pulse)
  if (coinPulses > 0 && (millis() - lastCoinTime > 500)) {
    int amount = coinPulses * 1; 
    totalBalance += amount;
    
    Serial.print("[COIN] Detected: "); Serial.print(coinPulses);
    Serial.print(" pulses | Added: PHP "); Serial.println(amount);
    Serial.print(">>> NEW BALANCE: PHP "); Serial.println(totalBalance);
    
    coinPulses = 0; // Reset for next coin
  }
}