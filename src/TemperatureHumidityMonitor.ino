//
// Arduino temperature & humidity monitor/logger
//
// ------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2014 Dan Cotora
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// ------------------------------------------------------------------------
//
// This Arduino code can be used to monitor the current temperature and humidity
// from a DHT sensor and log the data to an SD card.
//
// The program also relies on a DS1307 RTC for time keeping.
//
// Data is read and recorded on the SD card every 60 seconds.
//

#include <Wire.h>
#include <DHT.h>
#include <RTClib.h>
#include <SdFat.h>

#define DHTPIN 7
#define DHTVCCPIN 2
#define DHTTYPE DHT22
#define CSPIN 4
#define LEDPIN 9

RTC_DS1307 RTC;
DHT dht(DHTPIN, DHTTYPE);
SdFat sd;
SdFile csvFile;

void setup() {
  pinMode(DHTVCCPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);

  Serial.begin(9600);

  Serial.println("Initializing the microSD card...");
  while (!sd.begin(CSPIN, SPI_HALF_SPEED)) {
    sd.initErrorPrint();
    delay(1000);
  }

  Serial.println("Initializing RTC...");
  Wire.begin();
  RTC.begin();

  Serial.println("Initializing the DHT sensor...");
  dht.begin();
}

void loop() {
  // Power the DHT sensor
  digitalWrite(DHTVCCPIN, HIGH);
  digitalWrite(LEDPIN, HIGH);

  // Wait 1 second for the sensor and SD card to stabilize
  delay(1000);

  // Get the current date
  DateTime now = RTC.now();

  // Read the temperature and the humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Turn off the sensor
  digitalWrite(DHTVCCPIN, LOW);

  // Check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor");
    digitalWrite(LEDPIN, LOW);
    return;
  }

  // Open the CSV file for writing
  while (!csvFile.open("monitor.csv", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorPrint("Failed to open monitor.csv for writing");
    delay(1000);
  }

  // Print the current date to the console
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print("\t");

  // Print the temperature and the humidity in the console
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  // Write the current date in the CSV file
  csvFile.print(now.year(), DEC);
  csvFile.print('/');
  csvFile.print(now.month(), DEC);
  csvFile.print('/');
  csvFile.print(now.day(), DEC);
  csvFile.print(' ');
  csvFile.print(now.hour(), DEC);
  csvFile.print(':');
  csvFile.print(now.minute(), DEC);
  csvFile.print(':');
  csvFile.print(now.second(), DEC);
  csvFile.print(",");

  // Write the temperature and the humidity in the CSV file
  csvFile.print(h);
  csvFile.print(",");
  csvFile.println(t);

  // Close the file
  while (!csvFile.close()) {
    sd.errorPrint("Failed to close monitor.csv");
    delay(1000);
  }

  digitalWrite(LEDPIN, LOW);

  // Wait 60 seconds
  delay(60000);
}
