#pragma once
#include "RequestContext.h"
#include <Hash.h>
#include "ESP8266WiFi.h"
#include "rBase64.h"
#include "Hash.h"

class WebSocketContext
{
    friend class WebServer;
private:
    static constexpr const char * MASTER_KEY = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    RequestContext* request = nullptr;
    WiFiClient* client = nullptr;
    int bufferLength = 1024;
    void sendHeader();

    bool handle();

    String generateAcceptKey(const char* clientKey);
    char* receive();
    char* lastMessage = NULL;

    bool readOnly = false;
public:
    WebSocketContext(WiFiClient* client, RequestContext* request, int bufferLength = 1024);
    ~WebSocketContext();

    void send(const char *message);
    void send(String message);
    bool available();

    char* getMessage();

    void setReadOnly(bool readOnly);
    bool isReadOnly();
};
