#ifndef __CONNECTOR_BLE__
#define __CONNECTOR_BLE__

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Defines.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <Database/DatabaseHandler.hpp>
#include <Helper.hpp>

#define DEBUG_LOG 1

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_RX_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_TX_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

/*
//////Data Format//////
Requests:
    reqcall 0x0 - Get if device is online
    reqinfo 0x1 - Get device Information
    reqdata 0x2 - Get device data (var (0xX), from, to)
    reqfree 0x3 - Custom request
    reqware 0x4 - Get firmware version
Repsonses:
    respcall 0x0 - Empty
    respinfo 0x1 - Send basic device info
    respdata 0x2 - Send responce with variable data
    respware 0x3 - Send response with firmware version
Response Args:
    respinfo:
        NAME, TYPE, CUSTOM SET
    respdata:
        list 0x0 - Get list of data variables
        data 0xX - defined data X - prefix
//////Data Format//////
*/
class ConnectorBLE{
public:
    static int Init();
    static int SetData(std::string newdata);
    static int ReceiveData(){
        return 0;
    }
    static void SendData(std::string data);
    static void SendData_(std::string prefix);
    static void SendRequestedData(std::string date_from, std::string date_to, int position);
    static BLECharacteristic *pCharacteristic;
    
    class MyCallbacks: public BLECharacteristicCallbacks
    {
        BLECharacteristic *pCharacteristic;
        void onWrite(BLECharacteristic *pCharacteristic);
    };
    static std::string MakeCommand(char prefix, const char* args);
private:
    static bool old_message_sent;
    static char* BuildData();
    static std::string vars;
    static char char_buffer[600];
};

#endif