#include <Configurator.hpp>
#define DEBUG 1
// Initialize Static Variables
AsyncWebServer *Configurator::server;

void Configurator::Init() // Runs AsyncWebServer and handles communication
{
    IPAddress localIP(192, 168, 4, 22); // Configuring ip that can be accessed on AP mode
    IPAddress gateway(192, 168, 4, 9);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(localIP, gateway, subnet); // Configure AP
    WiFi.softAP("Meteostanice", "01234567"); // Starts AP

    Configurator::server = new AsyncWebServer(80); // Server Constructor
    // Requests
    Configurator::server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        request->send(SPIFFS, "/index.html", "text/html");
    });
    Configurator::server->on("/src/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                             { request->send(SPIFFS, "/src/style.css", "text/css"); });
    Configurator::server->on("/src/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
                             { request->send(SPIFFS, "/src/script.js", "text/js"); });
    Configurator::server->on("/database", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", DatabaseHandler::SelectAllTemps().c_str());
    });
    //std::string result = DatabaseHandler::SelectAll();
    //char *text = result.c_str();
    /*Configurator::server->on("/connect", HTTP_GET, [](AsyncWebServerRequest *request)
                             {
        if(Configurator::login && request->hasParam("mdp")){                        
            if(request->hasParam("ssid") && request->hasParam("pwd")){
                std::string ssid = (std::string)request->getParam("ssid")->value().c_str();
                std::string pwd = (std::string)request->getParam("pwd")->value().c_str();
                std::string md5 = (std::string)request->getParam("mdp")->value().c_str();
                Configurator::ssid_c = ssid;
                Configurator::pass_c = pwd;
                WiFi.begin(ssid.c_str(), pwd.c_str());
                unsigned long startTime = millis();
                while(WiFi.status() != WL_CONNECTED && millis() - startTime < 5000)
                {
                    esp_task_wdt_reset();
                    delay(50);
                }
                if(WiFi.status() == WL_CONNECTED)
                {
                    request->send_P(200, "text/plain", "connected");
                }
                else {request->send_P(200, "text/plain", "failure");Serial.println("Failed");}
            }else request->send_P(200, "text/plain", "failure");
        }
        else if(!Configurator::login){
            if(request->hasParam("ssid") && request->hasParam("pwd")){
                std::string ssid = (std::string)request->getParam("ssid")->value().c_str();
                std::string pwd = (std::string)request->getParam("pwd")->value().c_str();
                Configurator::ssid_c = ssid;
                Configurator::pass_c = pwd;
                xSemaphoreGive(Configurator::connectSemaphoreHandle);
                while (!xSemaphoreTake(Configurator::connectedSemaphoreHandle, 0))
                {
                    esp_task_wdt_reset();
                    delay(100);
                }
                if(WiFi.status() == WL_CONNECTED) request->send_P(200, "text/plain", "connected");
                else request->send_P(200, "text/plain", "failure");
            }else request->send_P(200, "text/plain", "failure"); 
        } });*/
    Configurator::server->begin(); // Start Server
}
