#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int UpperThreshold = 450;
int LowerThreshold = 350;
int reading = 0;
float BPM = 0.0;
float hr = 0.0;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;

const int buzzerPin = 8; // Define the buzzer pin

void setup() {
  Serial.begin(9600);
  pinMode(10, INPUT);
  pinMode(9, INPUT);
  pinMode(buzzerPin, OUTPUT); // Set the buzzer pin as an output
  lcd.begin(16, 2);
  analogWrite(6, 65); // Adjust contrast as needed
}

void loop() {
  if ((digitalRead(10) == 1) || (digitalRead(9) == 1)) {
    lcd.setCursor(0, 0);
    lcd.print("loose connection");
  } else {
    reading = analogRead(A0);

    // Heartbeat leading edge detected.
    if (reading > UpperThreshold && !IgnoreReading) {
      if (!FirstPulseDetected) {
        FirstPulseTime = millis();
        FirstPulseDetected = true;
      } else {
        SecondPulseTime = millis();
        PulseInterval = SecondPulseTime - FirstPulseTime;
        FirstPulseTime = SecondPulseTime;
      }
      IgnoreReading = true;
    }

    // Heartbeat trailing edge detected.
    if (reading < LowerThreshold) {
      IgnoreReading = false;
    }

    BPM = (1.0 / PulseInterval) * 60.0 * 1000;
    hr = BPM;
    Serial.println(reading);
    Serial.print("\t");
    Serial.print(BPM);
    Serial.println(" BPM");

    // Update LCD with BPM value
    lcd.setCursor(0, 0); // Adjust the LCD cursor position as needed
    lcd.print("Reading Started"); // Clear previous BPM value
    lcd.setCursor(0, 1); // Adjust the LCD cursor position as needed
    lcd.print("BPM: "); // Display the current BPM value
    lcd.print(BPM);

    // Check if BPM is out of range and activate buzzer if needed
    if (BPM < 60 || BPM > 125) {
      tone(buzzerPin, 1000); // Activate buzzer at 1000 Hz
    } else {
      noTone(buzzerPin); // Deactivate buzzer
    }

    delay(50);
  }
}
