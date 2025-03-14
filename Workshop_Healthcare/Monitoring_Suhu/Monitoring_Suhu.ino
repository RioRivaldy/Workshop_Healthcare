#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin D2 on the WeMos D1 Mini
#define ONE_WIRE_BUS D2
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass the oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);
// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(void)
{
  // Start serial communication
  Serial.begin(115200);
  Serial.println ("Dallas Temperature Control Library Demo");
  // Start up the library
  sensors.begin();
  // Initialize the LCD
  lcd.init();
  // Turn on the backlight
  lcd.backlight();
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
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistem monitor");
  lcd.setCursor(0, 1);
  lcd.print("Suhu Badan");
  delay(5000);
  lcd.clear();
  lcd.print("Temperature:");
}

void loop(void)
{
  // Request temperature readings
  sensors.requestTemperatures();
  // Read temperature in Celsius
  float tempC = sensors.getTempCByIndex(0);
  // Print temperature to serial monitor
  Serial.print("Temperature is: ");
  Serial.print(tempC);
  Serial.println("°C");
  // Print temperature to LCD
  lcd.setCursor(0, 1); // Set cursor to second line
  lcd.print(tempC);
  lcd.print(" C");
  delay(1000);
}
