#include <BLE/ConnectorBLE.hpp>

BLECharacteristic *ConnectorBLE::pCharacteristic;
std::string ConnectorBLE::vars = "";
bool ConnectorBLE::old_message_sent = true;
char ConnectorBLE::char_buffer[600];
int ConnectorBLE::Init()
{
    BLEDevice::init("api.ESP32-BLE-Server.hive");
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pService = pServer->createService(SERVICE_UUID);
    // vytvoření BLE komunikačního kanálu pro odesílání (TX)
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_TX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_READ);
    // vytvoření BLE komunikačního kanálu pro příjem (RX)
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_RX_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setCallbacks(new MyCallbacks());
    // zahájení BLE služby
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();

    BLEAdvertisementData advData;
    advData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    advData.setName("api.ESP32-BLE-Server.hive");
    pAdvertising->setAdvertisementData(advData);

    pAdvertising->start();
    return 0;
}
int ConnectorBLE::SetData(std::string newdata)
{
    vars = newdata;
    return 0;
}
std::string ConnectorBLE::MakeCommand(char prefix, const char *args)
{
    return (prefix + (std::string)(args)).c_str();
}
char *ConnectorBLE::BuildData()
{
    return "";
}
void ConnectorBLE::MyCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string value = pCharacteristic->getValue();
    char command = value.at(0);
    std::string data = value.substr(1, value.length());
    if (value.length() > 0)
    {
        switch (command)
        {
        case REQUEST_CALL:
            SendData(MakeCommand(RESPONSE_CALL, ""));
            break;
        case REQUEST_INFO:
            SendData(MakeCommand(RESPONSE_INFO, "ESP32,HIVE"));
            break;
        case REQUEST_WARE:
            SendData(MakeCommand(RESPONSE_WARE, "1.0"));
            break;
        case REQUEST_DATA:
            /*std::string from_date = Helper::split(data.c_str(), ';')[0];
            std::string to_date = Helper::split(data.c_str(), ';')[1];
            Serial.print("From: ");Serial.println(from_date.c_str());
            Serial.print("To: ");Serial.println(to_date.c_str());
            SendData(MakeCommand(RESPONSE_DATA, "Temperature_in:25.5,25.3,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.3,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5;Temperature_out:23.5,22.3,23.5,22.5,23.5,22.5,23.5,23.5,22.5,23.5,22.5,23.5,23.5,22.3,23.5,22.5,23.5,22.5,23.5,23.5,22.5,23.5,22.5,23.5;Humidity_in:25.5,25.3,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.3,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5;Humidity_out:23.5,22.3,23.5,22.5,23.5,22.5,23.5,23.5,22.5,23.5,22.5,23.5,23.5,22.3,23.5,22.5,23.5,22.5,23.5,23.5,22.5,23.5,22.5,23.5;Battery_in:0,15,30,45,60,70,80,30,45,60,70,80,0,15,30,45,60,70,80,30,45,60,70,80"));
            */
            break;
        case REQUEST_VARS:
            SendData(MakeCommand(RESPONSE_VARS, vars.c_str()));
            break;
        default:
            break;
        }
        if (command == REQUEST_DATA)
        {
            if (data == "last")
            {
            }
            else
            {
                std::string from_date = Helper::split(data.c_str(), ';')[0];
                std::string to_date = Helper::split(data.c_str(), ';')[1];
                int pos = std::atoi(Helper::split(data.c_str(), ';')[2].c_str());
                ConnectorBLE::SendRequestedData(from_date, to_date, pos);
                // SendData(MakeCommand(RESPONSE_DATA, "Temperature_in:25.5,25.3,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.3,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5;Temperature_out:23.5,22.3,23.5,22.5,23.5,22.5,23.5,23.5,22.5,23.5,22.5,23.5,23.5,22.3,23.5,22.5,23.5,22.5,23.5,23.5,22.5,23.5,22.5,23.5;Humidity_in:25.5,25.3,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.3,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5,25.5;Humidity_out:23.5,22.3,23.5,22.5,23.5,22.5,23.5,23.5,22.5,23.5,22.5,23.5,23.5,22.3,23.5,22.5,23.5,22.5,23.5,23.5,22.5,23.5,22.5,23.5;Battery_in:0,15,30,45,60,70,80,30,45,60,70,80,0,15,30,45,60,70,80,30,45,60,70,80"));
            }
        }
    }
}
void ConnectorBLE::SendData(std::string data)
{
    pCharacteristic->setValue(data);
    pCharacteristic->notify();
}
void ConnectorBLE::SendData_(std::string prefix)
{
    pCharacteristic->setValue(prefix + ConnectorBLE::char_buffer);
    pCharacteristic->notify();
}
void ConnectorBLE::SendRequestedData(std::string date_from, std::string date_to, int position)
{
    Serial.print("Position: ");
    Serial.println(position);
    memset(char_buffer, 0, 600);
    int delay_time = (int)(20.0f * position + 160.5789f);
    if (position == 0)
    {
        int count = DatabaseHandler::GetRequestCount(date_from, date_to); // Get Count of packets
        std::string pcount = std::to_string(count);
        if (pcount.length() < 3)
            pcount = "0" + pcount;
        if (pcount.length() < 3)
            pcount = "0" + pcount;
        SendData(MakeCommand(RESPONSE_DATA, ("000" + pcount).c_str())); // Send Initial packet
    }
    else
    {
        std::string position_ = std::to_string(position);
        if (position_.length() < 3)
            position_ = "0" + position_;
        if (position_.length() < 3)
            position_ = "0" + position_;
        std::string respprefix = "2";
        int state = DatabaseHandler::Select(date_from, date_to, position - 1, char_buffer);
        Serial.println("Sending data");
        SendData_(respprefix + position_); // Send Data packet
        Serial.print("Free Heap: ");
        Serial.println(ESP.getFreeHeap());
    }
    old_message_sent = true;
}
