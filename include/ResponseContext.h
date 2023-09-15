#pragma once
#include <ESP8266WiFi.h>
#include <map>

class ResponseContext
{
protected:
    friend class WebServer;
    void end();
private:
    WiFiClient *client = nullptr;
    std::map<const char *, const char *> headers;
    int code = 200;

    int step = 0;

    bool eventSource = false;

    void sendHeader();
    void sendBody(const char *body);
public:
    ResponseContext(WiFiClient *client);
    ~ResponseContext();

    WiFiClient* getClient();

    void setCode(int code);
    void setHeader(const char *key, const char *value);

    void send();
    void send(const char *body);
    void send(const char *body, int code);

    void setEventSource();
    void sendEvent(const char *event, const char *data);

    bool isEventSource();
};
