#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define SERIAL_BAUDRATE 115200
#define MAX30100_SAMPLERATE 10
#define MAX30100_BUFFERLENGTH 60

// Create an instance of the PulseOximeter class
PulseOximeter pox;

// Variables to track the last time the data was reported
uint32_t tsLastReport = 0;
float lastSpO2 = 0;
float lastHR = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUDRATE);

  // Initialize the I2C communication
  Wire.begin();

  // Initialize the pulse oximeter sensor
  pox.begin();
  
  // Set the current for the IR LED in the MAX30100 sensor
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void loop() {
  // Update the pulse oximeter readings
  pox.update();

  // Check if the sampling interval has passed
  if (millis() - tsLastReport > MAX30100_SAMPLERATE) {
    // Update the last recorded SpO2 and heart rate values
    lastSpO2 = pox.getSpO2();
    lastHR = pox.getHeartRate();

    // Add 1 to the SpO2 value (for example purposes)
    int lastSpO2Compute = lastSpO2 + 1;
            
    // Format the data as a string
    String data = String(lastSpO2Compute) + "," + String(lastHR) + "\n";

    // Print the data to the serial monitor
    Serial.print(data);

    // Update the timestamp for the last report
    tsLastReport = millis();    
  }
  
  // Short delay for stability
  delay(5);
}
