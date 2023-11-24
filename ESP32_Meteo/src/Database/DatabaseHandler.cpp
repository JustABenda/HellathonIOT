#include <Database/DatabaseHandler.hpp>

sqlite3 *DatabaseHandler::database;
sqlite3_stmt *DatabaseHandler::resource;
const char *DatabaseHandler::tail;
std::string *DatabaseHandler::varlist;
const char *DatabaseHandler::data = "Callback function called";
int DatabaseHandler::count = 0;
bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
int DatabaseHandler::Init(std::string variables[], std::string units[], std::string colors[], int count)
{ // Run as first command
    Serial.println("Init Sequence...");
    DatabaseHandler::count = count;
    varlist = variables;
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS Filesystem Error");
        return -1;
    } // Check if filesystem exists
    if (!SPIFFS.exists("/sqlite.db"))
    { // Ckecks if file exists
        Serial.println("Databse doesnt exist, creating one");
        File file = SPIFFS.open("/sqlite.db", FILE_WRITE); // Create file
        if (!file)
        { // Check for file existance again
            Serial.println("Error creating databse file");
            return -1;
        }
        int setup = SetupDatabase(); // Create Databse structure
        if (setup != 0)
        {
            Serial.println("Error during database setup");
            return setup;
        } // Throw error if setup failed
    }
    else
        int rc = sqlite3_open("/spiffs/sqlite.db", &DatabaseHandler::database); // Select Database
    Serial.println("Init successfull");
    return 0;
}
int DatabaseHandler::SetupDatabase()
{
    sqlite3_initialize();
    int rc = sqlite3_open("/spiffs/sqlite.db", &DatabaseHandler::database); // Select Database
    if (rc)
        Serial.println("Unable to open database file");
    std::string command = "CREATE TABLE data ('datetime' TEXT";
    for (int i = 0; i < count; i++)
    {
        command += ",'" + varlist[i] + "' FLOAT";
    }
    // command = command.substr(0, command.length()-1);
    command += ");";
    return Execute(command.c_str()); // Create Table to store variable types
}
int DatabaseHandler::Log(std::string datetime, float measurements[])
{
    std::string command = "INSERT INTO device ('datetime'";
    for (int i = 0; i < count; i++)
    {
        command += ",'" + varlist[i] + "'";
    }
    // command = command.substr(0, command.length()-1);
    command += ") VALUES ('" + datetime + "'";
    for (int i = 0; i < count; i++)
    {
        command += ",'" + std::to_string(measurements[i]) + "'";
    }
    command += ")";
    return Execute(command.c_str()); // Store data
}
int DatabaseHandler::Execute(const char *command)
{ // Fast execute
    char *errorMSG = 0;
    int rc = sqlite3_exec(DatabaseHandler::database, command, DatabaseHandler::callback, (void *)DatabaseHandler::data, &errorMSG); // Execute command
    if (rc != SQLITE_OK)
    {
        Serial.print("Failed executing command: ");
        Serial.println(errorMSG);
        return -1;
    } // Checks for any errors
    return 0;
}
int DatabaseHandler::callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    Serial.printf("%s: ", (const char *)data);
    for (i = 0; i < argc; i++)
    {
        Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    Serial.printf("\n");
    return 0;
}
std::string DatabaseHandler::SelectAll()
{
    std::string exec_string = "SELECT * FROM device";
    std::string result = "";
    int rc = sqlite3_prepare_v2(DatabaseHandler::database, exec_string.c_str(), 1000, &DatabaseHandler::resource, &DatabaseHandler::tail);
    if (rc != SQLITE_OK)
    {
        Serial.println("Failed executing command");
        return "";
    }
    while (sqlite3_step(DatabaseHandler::resource) == SQLITE_ROW)
    {
        std::string response = "";
        response += ((const char *)sqlite3_column_text(DatabaseHandler::resource, 0));
        response += "; ";
        for (int i = 1; i <= count; i++)
        {
            response += ((const char *)sqlite3_column_text(DatabaseHandler::resource, i));
            response += "; ";
        }
        Serial.println(response.c_str());
        result += response + "|";
    }
    sqlite3_finalize(DatabaseHandler::resource);
    return result;
}
std::string DatabaseHandler::SelectAllTemps()
{
    std::string exec_string = "SELECT datetime, temperature FROM device";
    std::string result = "";
    int rc = sqlite3_prepare_v2(DatabaseHandler::database, exec_string.c_str(), 1000, &DatabaseHandler::resource, &DatabaseHandler::tail);
    if (rc != SQLITE_OK)
    {
        Serial.println("Failed executing command");
        return "";
    }
    while (sqlite3_step(DatabaseHandler::resource) == SQLITE_ROW)
    {
        std::string response = "";
        response += ((const char *)sqlite3_column_text(DatabaseHandler::resource, 0));
        response += "@";
        response += ((const char *)sqlite3_column_text(DatabaseHandler::resource, 1));
        Serial.println(response.c_str());
        result += response + "|";
    }
    sqlite3_finalize(DatabaseHandler::resource);
    return result;
}
int DatabaseHandler::Select(std::string date_from, std::string date_to, int position, std::stringstream *ss_buffer)
{
    if (ss_buffer->bad())
        return -3;
    int number_to_start_at = position * SAFE_NUM_RECORDS;
    std::string exec_string = "SELECT * FROM device WHERE datetime BETWEEN '" + date_from + ":00' AND '" + date_to + ":00'";
    int rc = sqlite3_prepare_v2(DatabaseHandler::database, exec_string.c_str(), 1000, &DatabaseHandler::resource, &DatabaseHandler::tail);
    if (rc != SQLITE_OK)
    {
        Serial.println("Failed executing command");
        return -2;
    }
    int record_count = 0;
    while (sqlite3_step(DatabaseHandler::resource) == SQLITE_ROW)
    {
        if (record_count < number_to_start_at + SAFE_NUM_RECORDS && record_count >= number_to_start_at)
        { // Append data into buffer
            for (int i = 0; i < count; i++)
            {
                std::string data = "";
                data = (std::string)(const char *)sqlite3_column_text(DatabaseHandler::resource, i);
                replace(data, "000", "0");
                replace(data, "000", "0");
                (*ss_buffer) << data.c_str();
                (*ss_buffer) << ".";
            }
            std::string data = "";
            data = (std::string)(const char *)sqlite3_column_text(DatabaseHandler::resource, count);
            replace(data, "000", "0");
            replace(data, "000", "0");
            //Serial.println(data.c_str());
            (*ss_buffer) << data.c_str();
            (*ss_buffer) << ";";
            /*Serial.print(record_count);
            Serial.print(": ");
            Serial.println(ss_buffer->bad());*/
            delay(2);
        }
        record_count++;
    }
    sqlite3_finalize(DatabaseHandler::resource);
    return ss_buffer->bad() ? -1 : 0;
}
int DatabaseHandler::Select(std::string date_from, std::string date_to, int position, char* buffer)
{
    int number_to_start_at = position * SAFE_NUM_RECORDS;
    std::string exec_string = "SELECT * FROM device WHERE datetime BETWEEN '" + date_from + ":00' AND '" + date_to + ":00'";
    int rc = sqlite3_prepare_v2(DatabaseHandler::database, exec_string.c_str(), 1000, &DatabaseHandler::resource, &DatabaseHandler::tail);
    if (rc != SQLITE_OK)
    {
        Serial.println("Failed executing command");
        return -2;
    }
    int record_count = 0;
    while (sqlite3_step(DatabaseHandler::resource) == SQLITE_ROW)
    {
        if (record_count < number_to_start_at + SAFE_NUM_RECORDS && record_count >= number_to_start_at)
        { // Append data into buffer
            for (int i = 0; i < count; i++)
            {
                std::string data = "";
                data = (std::string)(const char *)sqlite3_column_text(DatabaseHandler::resource, i);
                replace(data, "000", "0");
                replace(data, "000", "0");
                strcat(buffer, data.c_str());
                strcat(buffer, "-");
            }
            std::string data = "";
            data = (std::string)(const char *)sqlite3_column_text(DatabaseHandler::resource, count);
            replace(data, "000", "0");
            replace(data, "000", "0");
            //Serial.println(data.c_str());
            strcat(buffer, data.c_str());
            strcat(buffer, ";");
            /*Serial.print(record_count);
            Serial.print(": ");
            Serial.println(ss_buffer->bad());*/
            delay(2);
        }
        record_count++;
    }
    sqlite3_finalize(DatabaseHandler::resource);
    return 0;
}
int DatabaseHandler::GetRequestCount(std::string date_from, std::string date_to)
{
    int real_count = DatabaseHandler::GetCount(date_from, date_to);
    int count_ = (int)(real_count / SAFE_NUM_RECORDS);
    if (real_count - count_ > 0)
        count_++;
    return count_;
}
int DatabaseHandler::GetCount(std::string date_from, std::string date_to)
{ // SELECT COUNT(*) from data
    std::string exec_string = "SELECT COUNT(*) FROM device WHERE datetime BETWEEN '" + date_from + ":00' AND '" + date_to + ":00'";
    int rc = sqlite3_prepare_v2(DatabaseHandler::database, exec_string.c_str(), 1000, &DatabaseHandler::resource, &DatabaseHandler::tail);
    if (rc != SQLITE_OK)
    {
        Serial.println("Failed executing command");
        return -1;
    }
    std::string response = "";
    while (sqlite3_step(DatabaseHandler::resource) == SQLITE_ROW)
    {
        response = ((const char *)sqlite3_column_text(DatabaseHandler::resource, 0));
        Serial.println(response.c_str());
    }
    sqlite3_finalize(DatabaseHandler::resource);
    return std::stoi(response);
}