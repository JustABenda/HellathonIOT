#pragma once
#include <unordered_map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <Preferences.h>
#include <esp_task_wdt.h>
#include <Database/DatabaseHandler.hpp>


using namespace std;
class Configurator
{
public:

    //static void Init(std::string title_, bool login_, std::string FOTA_URL_); // Init The server
    static void Init();
private:
    static AsyncWebServer *server;
    static void ReplaceAll(std::string &str, const std::string &from, const std::string &to);
};