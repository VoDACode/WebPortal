#pragma once
#include <ESP8266WiFi.h>
#include "ResponseContext.h"
#include "RequestContext.h"
#include "WebSocketContext.h"

class HttpContext
{
private:
    RequestContext *request;
    ResponseContext *response;
    WebSocketContext *websocket;
    WiFiClient *client;

    void handleRequest();
public:
    HttpContext(WiFiClient *client);
    ~HttpContext();

    RequestContext* getRequest();
    ResponseContext* getResponse();
    WebSocketContext* getWebSocket();

    bool isWebsocket();
    WiFiClient *getClient();
};