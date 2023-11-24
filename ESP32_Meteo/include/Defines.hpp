#pragma once
//Request from user
#define REQUEST_CALL '0'
#define REQUEST_INFO '1'
#define REQUEST_DATA '2'
#define REQUEST_FREE '3'
#define REQUEST_WARE '4'
#define REQUEST_VARS '5'
//Responses from device
#define RESPONSE_CALL '0'
#define RESPONSE_INFO '1'
#define RESPONSE_DATA '2'
#define RESPONSE_WARE '3'
#define RESPONSE_VARS '4'

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