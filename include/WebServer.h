#pragma once
#include "ESP8266WiFi.h"
#include "HttpContext.h"
#include <vector>
#include "rBase64.h"
#include <initializer_list>

using namespace std;

#define MAX_CLIENTS 100
const unsigned long SESSION_LIFETIME = 60 * 30 * 1000; // 30 minutes
using HttpHandleCallback = std::function<void(HttpContext &)>;

class WebServer
{
private:
    struct HandleRecord
    {
        const char *path;
        HttpMethod method;
        vector<HttpHandleCallback> callbacks;
        bool websocket;
        bool webSocketReadOnly;
    };

    WiFiServer *server;
    HttpContext *handlingContext = nullptr;
    vector<HttpContext *> clients[MAX_CLIENTS];
    int port;
    int clientCount;

    unsigned long lastAuthorizationActivity = 0;

    void handleClient(HttpContext *client);
    void manageClients();

    vector<HandleRecord *> callbacks;

    bool matchPath(const char *path, const char *pattern);
    bool matchMethod(HttpMethod method, HttpMethod pattern);

public:
    WebServer(int port = 80);
    ~WebServer();

    void begin();

    void handle();

    void onWs(const char *path, HttpHandleCallback callback, bool webSocketReadOnly = false);

    void on(const char *path, HttpMethod method, HttpHandleCallback callback);
    void on(const char *path, HttpMethod method, std::initializer_list<HttpHandleCallback> callbacks);

    void requestAuthentication(const char *username, const char *password);

    int getClientCount();

    void broadcast(const char *message);
    void broadcast(String message);
};
