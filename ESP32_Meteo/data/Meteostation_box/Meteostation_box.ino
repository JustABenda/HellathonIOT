#include <LowPower.h>

#include <TimerOne.h>

//#include <Wire.h>
#include <Adafruit_BMP280.h> //pressure
#include <Adafruit_Sensor.h>

#include  <PCF8583.h> //I2C counter

#include <SHT1x.h> //temperature&humidity

#include <SoftwareSerial.h> //communication with ESP8266

// ESP8266 serial and definitions
#define ESP8266_TX 8 
#define ESP8266_RX 9 
SoftwareSerial ESP8266 (ESP8266_RX, ESP8266_TX);
#define MAX_ESP8266_READ 15
bool ESP8266Connected = false;

// Sigfox serial and definitions
#define SIGFOX_TX 7 
#define SIGFOX_RX 6 
SoftwareSerial Sigfox(SIGFOX_RX, SIGFOX_TX);
#define MAX_SIGFOX_READ 50
bool sigfoxConnected = false;

// Specify data and clock connections and instantiate SHT1x object
#define dataPin 5//A4
#define clockPin 4//A5
SHT1x sht1x(dataPin, clockPin);

Adafruit_BMP280 bmp; // I2C
#define ALTITUDE 369

#define REPORT_PERIOD 75

PCF8583 rainCounter(0xA0);
PCF8583 windCounter(0xA2);

#define MS_PER_PULSE_SEC 0.6672
#define SPEED_PERIOD 8.0 

#define DIRECTION_INPUT A0
#define TOLERANCE 6


int localWindCounter = 0;
float windSpeed = 0;
int windSpeedCount = 0;
double windSpeedSum = 0;
float maxWindSpeed = 0;

float lastWindDirection = 0;
int lastWindDirectionShort = 0;

volatile int timeCounter = REPORT_PERIOD;

struct MeasuredData {
   float temp;
   float humidity;
   float absPressure;
   float pressure;
   float windSpeed;
   float maxWindSpeed;
   float windDirection;
   int windDirectionShort;
   float light;
   float rain;
   int battery;
} measuredData; 


//8s periodical calculate and store wind speed
void measureWindSpeed() {
  windSpeed = (localWindCounter / SPEED_PERIOD )* MS_PER_PULSE_SEC;
  Serial.print("Counter: ");
  Serial.print(localWindCounter);
  Serial.print(" Speed: ");
  Serial.println(windSpeed);
  windSpeedCount++;
  windSpeedSum += windSpeed;
  if (windSpeed > maxWindSpeed) {
    maxWindSpeed = windSpeed;
  }
}

//10min average wind speed calculation
void calculateWindSpeed(){
  if (windSpeedCount > 0)
    measuredData.windSpeed = windSpeedSum/windSpeedCount;
  else
    measuredData.windSpeed = 0;
  measuredData.maxWindSpeed = maxWindSpeed;
  Serial.print("Average speed: ");
  Serial.print(measuredData.windSpeed);
  Serial.print(" Max speed: ");
  Serial.println(measuredData.maxWindSpeed);
  windSpeedCount = 0;
  windSpeedSum = 0;
  maxWindSpeed = 0;
}

void measureWindDirection(){
  int sensorValue = analogRead(DIRECTION_INPUT);

  if (sensorValue < 785 + TOLERANCE && sensorValue > 785 - TOLERANCE){
    measuredData.windDirection = 0;
    measuredData.windDirectionShort = 0;
  }
  else if (sensorValue < 405 + TOLERANCE && sensorValue > 405 - TOLERANCE){
    measuredData.windDirection = 22.5;
    measuredData.windDirectionShort = 1;
  }
  else if (sensorValue < 460 + TOLERANCE && sensorValue > 460 - TOLERANCE){
    measuredData.windDirection = 45;
    measuredData.windDirectionShort = 2;
  }
  else if (sensorValue < 83 + TOLERANCE && sensorValue > 83 - TOLERANCE){
    measuredData.windDirection = 67.5;
    measuredData.windDirectionShort = 3;
  }
  else if (sensorValue < 93 + TOLERANCE && sensorValue > 93 - TOLERANCE){
    measuredData.windDirection = 90;
    measuredData.windDirectionShort = 4;
  }
  else if (sensorValue < 66 + TOLERANCE && sensorValue > 66 - TOLERANCE){
    measuredData.windDirection = 112.5;
    measuredData.windDirectionShort = 5;
  }
  else if (sensorValue < 184 + TOLERANCE && sensorValue > 184 - TOLERANCE){
    measuredData.windDirection = 135;
    measuredData.windDirectionShort = 6;
  }
  else if (sensorValue < 126 + TOLERANCE && sensorValue > 126 - TOLERANCE){
    measuredData.windDirection = 157.5;
    measuredData.windDirectionShort = 7;
  }
  else if (sensorValue < 287 + TOLERANCE && sensorValue > 287 - TOLERANCE){
    measuredData.windDirection = 180; 
    measuredData.windDirectionShort = 8;  
  }
  else if (sensorValue < 244 + TOLERANCE && sensorValue > 244 - TOLERANCE){
    measuredData.windDirection = 202.5;
    measuredData.windDirectionShort = 9;
  }
  else if (sensorValue < 629 + TOLERANCE && sensorValue > 629 - TOLERANCE){
    measuredData.windDirection = 225;
    measuredData.windDirectionShort = 10;
  }
  else if (sensorValue < 598 + TOLERANCE && sensorValue > 598 - TOLERANCE){
    measuredData.windDirection = 247.5;
    measuredData.windDirectionShort = 11;
  }
  else if (sensorValue < 944 + TOLERANCE && sensorValue > 944 - TOLERANCE){
    measuredData.windDirection = 270;
    measuredData.windDirectionShort = 12;
  }
  else if (sensorValue < 826 + TOLERANCE && sensorValue > 826 - TOLERANCE){
    measuredData.windDirection = 292.5;
    measuredData.windDirectionShort = 13;
  }
  else if (sensorValue < 886 + TOLERANCE && sensorValue > 886 - TOLERANCE){
    measuredData.windDirection = 315;
    measuredData.windDirectionShort = 14;
  }
  else if (sensorValue < 702 + TOLERANCE && sensorValue > 702 - TOLERANCE){
    measuredData.windDirection = 337.5;
    measuredData.windDirectionShort = 15;  
  }
  else {
    measuredData.windDirection = lastWindDirection;
    measuredData.windDirectionShort = lastWindDirectionShort; 
  }

  lastWindDirection = 0;
  lastWindDirectionShort = 0;
  // print the results to the Serial Monitor:
  Serial.print("Wind direction: ");
  Serial.println(measuredData.windDirection);
}



int measureTempHum() {
    measuredData.humidity = sht1x.readHumidity();
    measuredData.temp = sht1x.readTemperatureC();

    // Check if any reads failed and exit early (to try again).
    if (isnan(measuredData.humidity) || isnan( measuredData.temp)) {
      Serial.println("Failed to read from DHT sensor!");
      return 1;
    }

    Serial.print("Humidity: ");
    Serial.print(measuredData.humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(measuredData.temp);
    Serial.print(" *C \n");
    return 0;
}

void measurePressure(){
    float abs_pressure = bmp.readPressure();
    Serial.print("Abs. pressure = ");
    Serial.print(abs_pressure);
    Serial.println(" Pa");

    float rel_pressure = abs_pressure * pow((1 - 0.0065*ALTITUDE / (measuredData.temp + 0.0065*ALTITUDE + 273.15)),-5.257);

    Serial.print("Rel. pressure = ");
    Serial.print(rel_pressure);
    Serial.println(" Pa");

    measuredData.pressure = rel_pressure/100.0;//rel_pressure;
    measuredData.absPressure = abs_pressure/100.0;
}


void measureRain(){
    measuredData.rain = rainCounter.getCount() * 0.2794; 
    rainCounter.setCount(0); 
    Serial.print("Rain = ");
    Serial.print(measuredData.rain);
    Serial.println(" mm/10min");
}

String createReportData() {
    String data = dec2bin(int(measuredData.temp*10),10) + dec2bin(int(measuredData.humidity*10),10) + dec2bin(int(measuredData.absPressure*10),14) + dec2bin(int(measuredData.rain*10),10) + dec2bin(int(measuredData.windSpeed*10),9) + dec2bin(int(measuredData.maxWindSpeed*10),9) + dec2bin(measuredData.windDirectionShort,4) + dec2bin(measuredData.battery,6);
    Serial.println(data);

    String data_hex = "";
    String byte_hex = "";
    for(int i=0; i<data.length()/8; i++){
      byte_hex = String(bin2dec(data.substring(i*8, (i+1)*8)),HEX);
      if (byte_hex.length() < 2) {
        byte_hex = "0" + byte_hex;
      }
      data_hex = data_hex + byte_hex;
      byte_hex = "";
    }
    Serial.println(data_hex);
    return data_hex;
}

void printResponseESP8266() {
  while (ESP8266.available()) {
    String str = ESP8266.readStringUntil('\n');
    //String str = ESP8266.readString();
    Serial.print("responseESP: "); 
    Serial.println(str); 
  }
}

void reportESP8266(String data_hex){
  digitalWrite(10, HIGH);
  delay(1000);

  int i = 0;
  while (! ESP8266.available()) {
    i++;
    LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    if (i > MAX_ESP8266_READ) {
      Serial.println("ERROR: ESP8266 before report");
      return;
    }
  };
  
  printResponseESP8266();

  String cmd = "AT$SEND=" + data_hex;
  Serial.println(cmd);
  ESP8266.println(cmd);

  i = 0;
  while (! ESP8266.available()) {
    i++;
    LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    if (i > MAX_ESP8266_READ) {
      Serial.println("ERROR: ESP8266 after report");
      return;
    }
  };
  

  printResponseESP8266();
  Serial.println("precteno");
  digitalWrite(10, LOW);
}

bool connectESP8266(){
  digitalWrite(10, HIGH);
  delay(1000);
    
  int i = 0;
  while (! ESP8266.available()) {
    i++;
    LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    if (i > MAX_ESP8266_READ) {
      Serial.println("ERROR: ESP8266 before connect");
      return false;
    }
  };
  
  printResponseESP8266();
  
  String cmd = "AT$SETTINGS";
  Serial.println(cmd);
  ESP8266.println(cmd);
  //delay(5000);
  i = 0;
  while (! ESP8266.available()) {
    i++;
    LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    if (i > MAX_ESP8266_READ) {
      Serial.println("ERROR: ESP8266 wait for connection");
      return false;
    }
  };
  printResponseESP8266();
  Serial.println("prectenoConnect");
  digitalWrite(10, LOW);
  return true;
}

int getSigfoxVoltage() {
  Sigfox.print("AT$V?\n");
  int i = 0;
  while (! Sigfox.available()) {
    i++;
    if (i > MAX_SIGFOX_READ) {
      Serial.println("ERROR: Sigfox get voltage");
      return 0;
    }
    LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
  };
  String response = Sigfox.readString();
  String voltage = response.substring(0,2);
  
  Serial.print("Voltage: ");
  Serial.println(voltage);

  return voltage.toInt();
}

int sigfoxSleep() {
  Sigfox.print("AT$P=1\n");
  int i = 0;
  while (! Sigfox.available()) {
    i++;
    LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    if (i > MAX_SIGFOX_READ) {
      Serial.println("ERROR: Sigfox go to sleep");
      return 1;
    }
  };
  Serial.print(Sigfox.readString());
  return 0;
}

bool isSigfoxConnected() {
  Sigfox.print("AT\n");
  int i = 0;
  while (! Sigfox.available()) {
    i++;
    LowPower.idle(SLEEP_250MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    if (i > MAX_SIGFOX_READ) {
      Serial.print("ERROR: Sigfox connected:");
      Serial.println(i);
      return false;
    }
  };
  Serial.print(Sigfox.readString());
  Serial.println(i);
  return true;
}

void sigfoxWakeUp() {
  Sigfox.listen();
  Sigfox.print("\n");
  delay(50);
}

int sigfoxReportData(String data_hex) {
  data_hex = "AT$SF=" + data_hex + "\n";
  Serial.println(data_hex);

  char *str = data_hex.c_str();
  
  Sigfox.print(str);
  Serial.println();
  delay(50);
  
  int i = 0;
  while (! Sigfox.available()) {
    i++;
    if (i > MAX_SIGFOX_READ) {
      Serial.println("ERROR: Sigfox report");
      return 1;
    }
    //delay(1000);
    LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
  };
  Serial.print(Sigfox.readString());
  delay(50);
  
  return 0;
}


int bin2dec(String bin){
  int dec = 0;
  for (int i =0; i < 8; i++) {
    dec = dec | (bin[i] - '0') << (7-i);
  }
  return dec;
}

String dec2bin(int x, int n) {
  String bin = "";
  for(int i=0; i<n; i++){
    bin = bitRead(x,i) + bin;
  }
  return bin;
}

void setup() {
  pinMode(10, OUTPUT);//controll pin for GPS power
  digitalWrite(10, LOW);
  
  Serial.begin(115200);
  Serial.println("Starting");

  //serial for ESP8266 module
  ESP8266.begin(9600);
  ESP8266.listen();
  ESP8266Connected = connectESP8266();
  Serial.print("ESP connected: ");
  Serial.println(ESP8266Connected);

  //delay(500);
  //serial for SIGFOX module
  Sigfox.begin(9600);
  Sigfox.setTimeout(10);

  Sigfox.listen();

  sigfoxWakeUp();
  sigfoxConnected = isSigfoxConnected();
  Serial.print("Sigfox connected:");
  Serial.println(sigfoxConnected);

  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
  }

  rainCounter.setMode(MODE_EVENT_COUNTER);
  rainCounter.setCount(0);
  windCounter.setMode(MODE_EVENT_COUNTER);
  windCounter.setCount(0); 
}

void loop() { 
  if (timeCounter >= REPORT_PERIOD-2) {
    delay(100);
    //measure weather values
    measureWindDirection();
    calculateWindSpeed();
    measureTempHum();
    measurePressure();
    measureRain();

    Serial.println("Values measured.");
    
    if (sigfoxConnected){
      //wake up sigfox and measure battery
      Sigfox.listen();
      sigfoxWakeUp();
      measuredData.battery = getSigfoxVoltage();
    }

    //create report data
    String reportData = createReportData();

    if (sigfoxConnected){
      //report Sigfox and go to sleep
      sigfoxReportData(reportData);
      delay(40); //delay before sigfox sleep
      sigfoxSleep();
      Serial.println("Sigfox reported and slept.");
    }

    if (ESP8266Connected){
      //report ESP8266
      ESP8266.listen();
      reportESP8266(reportData);
    }
    
    timeCounter = 0; 

  }
  
  delay(100);
  windCounter.setCount(0);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  //every 8 seconds measure wind speed
  localWindCounter = windCounter.getCount();
  measureWindSpeed();
  timeCounter++;
}


