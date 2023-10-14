#pragma once
#include <ESP8266WiFi.h>
#include <map>
#include "config.h"
#include "LittleFS.h"

class ResponseContext
{
protected:
    friend class WebServer;
private:
    WiFiClient *client = nullptr;
    std::map<const char *, const char *> headers;
    int code = 200;

    int step = 0;

    bool eventSource = false;
    bool ended = false;

    void sendHeader();
    void sendBody(const char *body);
    void sendBodyFromFile(const char *filename);
public:
    ResponseContext(WiFiClient *client);
    ~ResponseContext();

    WiFiClient* getClient();

    void setCode(int code);
    void setHeader(const char *key, const char *value);

    void send();
    void send(const char *body);
    void send(const char *body, int code);

    void sendFileContent(const char *filename);
    void sendFileContent(const char *filename, int code);

    void end();
    bool isEnded();

    void unauthorized(const char *message = "Unauthorized");

    void setEventSource();
    void sendEvent(const char *event, const char *data);

    bool isEventSource();
};
