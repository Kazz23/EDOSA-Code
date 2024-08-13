#include <SFM3X00.h>
#include <SFM4X00.h>
#include <Wire.h>

// Address of the airflow sensor
// Usually 64 or 0x40 by default
#define AIRFLOW_SENSOR_ADDRESS 0x40

#include "SFM3X00.h"

// Create an instance of the sensor with the specified address
SFM3X00 AirFlowSensor(AIRFLOW_SENSOR_ADDRESS);

// Pin definitions
const int TempPin = A0; // TMP35 sensor pin
const int MicPin = A2;  // Mic sensor pin
const int ECGPin = A1;  // ECG sensor pin

// Initialize variables for sensor data
float BPM = 0;          // Beats Per Minute
float Dugoxy = 0;       // Blood Saturation
float spo2 = 0;         // SPO2 value
float hr = 0;           // Heart Rate

int RawSoundData = 0;   // Raw sound data
int RawTempValue = 0;   // Raw temperature value

void setup() {
  // Establish serial communication
  Wire.begin();
  Serial.begin(115200);

  // Set pin modes
  pinMode(TempPin, INPUT); // Setup for TMP35 sensor
  pinMode(MicPin, INPUT);  // Setup for mic sensor
  pinMode(ECGPin, INPUT);  // Setup for ECG sensor
  
  Serial.print("\n");

  // Initialize the airflow sensor and start measuring flow
  AirFlowSensor.begin();
}

void loop() {
  // Read airflow from the sensor
  float airflow = AirFlowSensor.readFlow();

  // Read temperature from the TMP35 sensor
  float RawTempValue = analogRead(TempPin);
  float TempValue = (RawTempValue * 500) / 1024.0; // Convert raw value to Celsius
  float TempCeltomV = TempValue * 10;              // Convert Celsius to millivolts

  // Read sound data from the mic sensor
  int RawSoundData = analogRead(MicPin);
  int SoundDataMap = map(RawSoundData, 0, 1023, 0, 255); // Map raw data to 0-255 range

  // Read ECG data from the ECG sensor
  int ECGData = analogRead(ECGPin);
  ECGData = ECGData >> 2; // Scale down ECG data

  // Check if data is available from Serial (e.g., NodeMCU)
  if (Serial.available()) {
    // Wait for data from NodeMCU
    String msg = Serial.readStringUntil('\n');
    float spo2 = msg.substring(0, msg.indexOf(',')).toFloat();
    float hr = msg.substring(msg.indexOf(',') + 1).toFloat();

    // Update BPM if the heart rate (hr) is within a valid range
    if (hr >= 40 && hr <= 400) {
      BPM = hr;
    }
    // Update blood saturation (Dugoxy) if spo2 is within a valid range
    if (spo2 >= 40 && spo2 <= 100) {
      Dugoxy = spo2;
    }

    // Print sensor data to the serial monitor
    Serial.print(ECGData);
    Serial.print("\t");
    Serial.print(airflow);
    Serial.print("\t");
    Serial.print(SoundDataMap);
    Serial.print("\t");
    Serial.print(TempValue);
    Serial.print("\t");
    Serial.print(Dugoxy);
    Serial.print("\t");
    Serial.println(BPM);
    Serial.print("\n");
  }

  delay(5); // Short delay for stability
}
