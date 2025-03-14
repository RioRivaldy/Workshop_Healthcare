#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// Define the data pin for the DS18B20 sensor
#define ONE_WIRE_BUS D2

// WiFi credentials
const char* ssid = "UGMURO-INET";
const char* password = "Gepuk15000";

// ThingSpeak settings
unsigned long myChannelNumber = 2530978;
const char* myWriteAPIKey = "2YWLDP7Z2Z05NQ6D";

// Create an instance of the MAX30105 class to interact with the sensor
MAX30105 particleSensor;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Define the size of the rates array for averaging BPM; can be adjusted for smoother results
const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is a good starting point.
byte rates[RATE_SIZE]; // Array to store heart rate readings for averaging
byte rateSpot = 0; // Index for inserting the next heart rate reading into the array
long lastBeat = 0; // Timestamp of the last detected beat, used to calculate BPM
float beatsPerMinute; // Calculated heart rate in beats per minute
int beatAvg; // Average heart rate after processing multiple readings
WiFiClient client;

void setup()
{
  Serial.begin(115200); // Start serial communication at 115200 baud rate
  Serial.println("Initializing...");
  // Start up the temperature sensor library
  sensors.begin();
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  // Attempt to initialize the MAX30105 sensor. Check for a successful connection and report.
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Start communication using fast I2C speed
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
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the rates array
      rateSpot %= RATE_SIZE; // Wrap the rateSpot index to keep it within the bounds of the rates array
      // Compute the average of stored heart rates to smooth out the BPM
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  // Request temperature readings
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0); // Get temperature in Celsius
  // Output the current IR value, BPM, averaged BPM, and temperature to the serial monitor
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  Serial.print(", Temp=");
  Serial.print(temperatureC);
  Serial.println("°C");

  // Send data to ThingSpeak
  ThingSpeak.setField(3, beatsPerMinute);
  ThingSpeak.setField(4, beatAvg);
  ThingSpeak.setField(5, temperatureC);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  // Delay to update the display and avoid flickering
  delay(15000); // ThingSpeak allows updates every 15 seconds (free accounts)
}
