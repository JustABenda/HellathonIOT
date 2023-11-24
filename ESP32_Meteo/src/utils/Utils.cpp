#include <utils/Utils.hpp>

std::vector<std::string>  Utilities::tokenize(std::string const &str, const char delim)
{
    std::vector<std::string> out;
    std::stringstream ss(str);

    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
    return out;
}
std::vector<std::string> Utilities::split_string(std::string text, char delimiter){
    std::vector<std::string> result {};
    std::istringstream sstream(text);
    std::string word = "";
    while(std::getline(sstream, word, delimiter)){
        word.erase(std::remove_if(word.begin(), word.end(), ispunct), word.end());
        result.push_back(word);
    }
    return result;
}
bool Utilities::isNumeric(std::string value){
    std::string::const_iterator it = value.begin();
    while (it != value.end() && std::isdigit(*it)) ++it;
    return !value.empty() && it == value.end();
}
int Utilities::hex_to_int(std::string hex){
    int x;   
    std::stringstream ss;
    ss << std::hex << hex.c_str();
    ss >> x;
    return x;
}
std::string Utilities::createDeafultMAC(){
    String default_mac = WiFi.macAddress();
    default_mac.toLowerCase();

    int mac_address[6];

    std::string str_mac = (std::string)default_mac.c_str();
    std::vector<std::string> mac_vector = Utilities::split_string(str_mac, ':');
    for(int i = 0; i < 6; i++) mac_address[i] = (int)Utilities::hex_to_int(mac_vector[i]);

    mac_address[5] = mac_address[5] + 0x01;

    //Create string
    std::stringstream buffer;
    buffer << std::hex << mac_address[0];
    for(int i = 1; i < 6; i++) buffer << ":" << std::hex << mac_address[i];

    return buffer.str();
}