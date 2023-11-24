#ifndef __DATABASE_HANDLER__
#define __DATABASE_HANDLER__

#include <unordered_map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "WiFi.h"
#include "SPIFFS.h"
#include <Preferences.h>
#include <sqlite3.h>
#include <utils/Utils.hpp>

#define SAFE_NUM_RECORDS 10

class DatabaseHandler{
public:
    static int Init(std::string variables[], std::string units[], std::string colors[], int count);
    static int CheckDatabase();
    static int Log(std::string datetime, float measurements[]);
    //static int Select(std::string date_from, std::string date_to, std::stringstream* ss_buffer);
    //static std::string Select(std::string date_from, std::string date_to, int max);
    static int Select(std::string date_from, std::string date_to, int position, std::stringstream* ss_buffer);
    static int Select(std::string date_from, std::string date_to, int position, char* buffer);
    static int GetRequestCount(std::string date_from, std::string date_to);
    static int GetCount(std::string date_from, std::string date_to);
    static int Execute(const char* command);
    static std::string SelectAll();
    static std::string SelectAllTemps();
private:
    static int count;
    static std::string *varlist;
    static int SetupDatabase();
    static sqlite3 *database;
    static sqlite3_stmt *resource;
    static const char *tail;
    static int callback(void *data, int argc, char **argv, char **azColName);
    static const char* data;
};

#endif