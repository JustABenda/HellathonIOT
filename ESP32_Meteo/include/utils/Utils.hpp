#ifndef __UTILS__
#define __UTILS__

#include <Arduino.h>
#include <string>
#include <vector>
#include "unordered_map"
#include <iostream>
#include <sstream>
#include <iterator>
#include <WiFi.h>

class Utilities{
public:
    static std::vector<std::string> tokenize(std::string const &str, const char delim);
    static std::vector<std::string> split_string(std::string text, char delimiter);
    static bool isNumeric(std::string value);
    static int hex_to_int(std::string hex);
    static std::string createDeafultMAC();
};

#endif