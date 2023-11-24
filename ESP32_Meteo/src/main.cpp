#include <Arduino.h>
#include <PCF8583C.hpp>
#include <Rtc_Pcf8563.h>
#include <Database/DatabaseHandler.hpp>
#include <BLE/ConnectorBLE.hpp>
#include <Configurator.hpp>

#define SCL_RTC 19
#define SDA_RTC 18

#define SCL_COUNTERS 22
#define SDA_COUNTERS 21

#define RTC_ADDR 0x51

#define COUNTER_RAIN_ADDR 0x50
#define COUNTER_WIND_ADDR 0x51

#define MODE_PIN 34
#define MODE_UI_PIN 35
#define FORCE_MEASURE_PIN 32

enum OperationMode{
  NORMAL,
  BLE,
  WIFI
};

PCF8583C *counter_rain;
PCF8583C *counter_wind;

Rtc_Pcf8563 rtc;

OperationMode operation_mode;

float temperature = 0;
float humidity = 0;
float wind_speed = 0;
short wind_direction = 0;
float rain_index = 0;

void InitSensors(){
  counter_rain = new PCF8583C(SDA_COUNTERS, SCL_COUNTERS, COUNTER_RAIN_ADDR);
  counter_wind = new PCF8583C(SDA_COUNTERS, SCL_COUNTERS, COUNTER_WIND_ADDR);
  counter_rain->reset();
  counter_wind->reset();
}

void SelectMode(){
  operation_mode = digitalRead(MODE_PIN) ? OperationMode::NORMAL : digitalRead(MODE_UI_PIN) ? OperationMode::BLE : OperationMode::WIFI;
}

void StartMode(){
  if(operation_mode == OperationMode::NORMAL){

  }
  else if(operation_mode == OperationMode::BLE){
    ConnectorBLE::Init();
    ConnectorBLE::SetData("Humidity_out.%.red;Temperature_out.°C.blue;WindSpeed_out.km/h.purple;WindDirection_out_text. .text;RainIndex_out.mm/m2.green");
  }
  else if(operation_mode == OperationMode::WIFI){
    Configurator::Init();
  }
}
void Measure(){
  
}
void setup() {
  Serial.begin(115200);
  Serial.println("Starting Meteo-station");

  if (!SPIFFS.begin()) // Load SPIFFS Filesystem
  {
    Serial.println("Failed to mount device filesystem"); // Failed To mount FS, probably doesn't exist
  }

  std::string vars[] = {"Humidity_out", "Temperature_out", "WindSpeed_out", "WindDirection_out_text", "RainIndex_out"};
  std::string units[] = {"%", "°C", "km/h", " ", "mm/m2"};
  std::string colors[] = {"red", "blue", "purple", "text", "green"};

  DatabaseHandler::Init(vars, units, colors, 5);
  
  InitSensors();

  pinMode(MODE_PIN, INPUT);
  pinMode(MODE_UI_PIN, INPUT);
  pinMode(FORCE_MEASURE_PIN, INPUT);
  SelectMode();
  StartMode();
  /*
  rtc.initClock();
  //set a time to start with.
  //day, weekday, month, century(1=1900, 0=2000), year(0-99)
  rtc.setDate(14, 6, 3, 1, 10);
  //hr, min, sec
  rtc.setTime(1, 15, 0);*/
}

void loop() {
  Measure();
  Serial.print(rtc.formatTime());
  Serial.print("\r\n");
  Serial.print(rtc.formatDate());
  Serial.print("\r\n");
  delay(100);
}
