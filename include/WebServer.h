#pragma once
#include "ESP8266WiFi.h"
#include "HttpContext.h"
#include <vector>

using namespace std;

#define MAX_CLIENTS 100

typedef function<void(HttpContext& )> HttpHandleCallback;

class WebServer
{
private:
    struct HandleRecord
    {
        const char *path;
        HttpMethod method;
        HttpHandleCallback callback;
        bool websocket;
        bool webSocketReadOnly;
    };
    

    WiFiServer *server;
    vector<HttpContext*> clients[MAX_CLIENTS];
    int port;
    int clientCount;
    void handleClient(HttpContext *client);

    void manageClients();

    vector<HandleRecord*> callbacks;

    bool matchPath(const char *path, const char *pattern);
    bool matchMethod(HttpMethod method, HttpMethod pattern);
public:
    WebServer(int port = 80);
    ~WebServer();

    void begin();

    void handle();

    void on(const char *path, HttpMethod method, HttpHandleCallback callback, bool websocket = false, bool webSocketReadOnly = false);
    void on(const char *path, HttpHandleCallback callback, bool websocket = false, bool webSocketReadOnly = false);

    int getClientCount();

    void broadcast(const char *message);
    void broadcast(String message);
};
