Arduino temperature & humidity monitor/logger
=============================================

This Arduino code can be used to monitor the current temperature and humidity from a DHT sensor ([DHT22](https://www.google.com/#q=DHT22) / [DHT11](https://www.google.com/#q=DHT11)) and log the data to an SD card. The program also relies on a [DS1307 RTC](https://www.google.com/#q=DS1307+RTC) connected over I2C for time keeping.

The data is read and recorded on the SD card every 60 seconds. While the data is read & written, an LED connected to D9 is also flashed to indicate activity.

If you want to use this code in your own project, make sure you correctly set the DHT pins/type as well as the CS pin for the SD card.

```arduino
#define DHTPIN 7
#define DHTVCCPIN 2
#define DHTTYPE DHT22
#define CSPIN 4
#define LEDPIN 9
```

The code uses the following libraries:

    - DHTlib by Adafruit Industries
    - RTCLib by JeeLabs
    - SdFat by William Greiman

All the libraries can be found inside the `libraries/` directory.