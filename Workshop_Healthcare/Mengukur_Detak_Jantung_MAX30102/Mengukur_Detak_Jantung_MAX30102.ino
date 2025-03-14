// Include necessary libraries for communication and for the MAX30105 sensor
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <LiquidCrystal_I2C.h>

MAX30105 particleSensor; // Create an instance of the MAX30105 class to interact with the sensor
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

// Define the size of the rates array for averaging BPM; can be adjusted for smoother results
const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is a good starting point.
byte rates[RATE_SIZE]; // Array to store heart rate readings for averaging
byte rateSpot = 0; // Index for inserting the next heart rate reading into the array
long lastBeat = 0; // Timestamp of the last detected beat, used to calculate BPM
float beatsPerMinute; // Calculated heart rate in beats per minute
int beatAvg; // Average heart rate after processing multiple readings

void setup()
{
  Serial.begin(115200); // Start serial communication at 115200 baud rate
  Serial.println("Initializing...");
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" WELCOME ");
  lcd.setCursor(0, 1);
  lcd.print(" TO ");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PSMURO");
  lcd.setCursor(0, 1);
  lcd.print("UNIV.GUNADARMA");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MCU HEALTHCARE");
  lcd.setCursor(0, 1);
  lcd.print("HEART RATE");
  delay(5000);
  lcd.clear();

  // Attempt to initialize the MAX30105 sensor. Check for a successful connection and report.
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    // Start communication using fast I2C speed
    Serial.println("MAX30102 was not found. Please check wiring/power.");
    while (1); // Infinite loop to halt further execution if sensor is not found
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  particleSensor.setup(); // Configure sensor with default settings for heart rate monitoring
  particleSensor.setPulseAmplitudeRed(0x0A); // Set the red LED pulse amplitude (intensity) to a low value as an indicator
  particleSensor.setPulseAmplitudeGreen(0); // Turn off the green LED as it's not used here
}

void loop()
{
  long irValue = particleSensor.getIR(); // Read the infrared value from the sensor
  if (checkForBeat(irValue) == true)
  { // Check if a heart beat is detected
    long delta = millis() - lastBeat; // Calculate the time between the current and last beat
    lastBeat = millis(); // Update lastBeat to the current time
    beatsPerMinute = 60 / (delta / 1000.0); // Calculate BPM
    // Ensure BPM is within a reasonable range before updating the rates array
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the rates array
      rateSpot %= RATE_SIZE; // Wrap the rateSpot index to keep it within the bounds of the rates array
      // Compute the average of stored heart rates to smooth out the BPM
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  if (irValue < 50000)
  {
    Serial.print(" No finger?");
    lcd.setCursor(0, 0);
    lcd.print("No Finger ? ");
    lcd.setCursor(0, 1);
    lcd.print("Letakan Jari! ");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BPM: ");
    lcd.print(beatsPerMinute);
    lcd.setCursor(0, 1);
    lcd.print("Avg BPM: ");
    lcd.print(beatAvg);
  }
  Serial.println();
  //delay(500); // Delay to update the display and avoid flickering
}
