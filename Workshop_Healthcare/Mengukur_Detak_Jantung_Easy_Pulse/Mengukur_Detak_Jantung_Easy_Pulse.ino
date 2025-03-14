#include <ESP8266WiFi.h>

int adc;
boolean counter;
int count;
int nilai_bpm;
unsigned long millisBefore;
unsigned long beatTime = 20000;
const int threshold = 500;

const char *ssid = "UGMURO-1";
const char *password = "Piscok2000";

void setup()
{
  Serial.begin(115200);
  counter = true;
  millisBefore = millis();
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
void loop()
{
  nilai_bpm = (count * (60000 / beatTime));
  adc = analogRead(A0);
  //Serial.println(adc);
  delay(10);
  if ((millis() - millisBefore) < beatTime)
  {
    if (counter == true)
    {
      if (adc >= threshold)
      {
        count++;
        counter = false;
        Serial.print("Beat : ");
        Serial.println(count);
      }
    }
    if (adc < threshold)
    {
      counter = true;
    }
  }
  else
  {
    Serial.print(nilai_bpm);
    Serial.println(" BPM");
    count = 0;
    millisBefore = millis();
    delay(4000);
  }
}
