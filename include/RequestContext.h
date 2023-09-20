#pragma once
#include <ESP8266WiFi.h>
#include <map>
#include <string>

using namespace std;

enum HttpMethod
{   
    ANY = -1,
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS,
    CONNECT,
    TRACE,
};

class RequestContext
{
private:
    WiFiClient *client = nullptr;
    std::map<string, string> headers;
    char *body = nullptr;
    std::map<string, string> queryParameters;
    char* path;
    HttpMethod method;
    bool websocket = false;

    string readHeader();
    int parsePathAndMethod(string header);
    void parseQueryParameters();
    void parseHeader(string header, int firstLineEnd);
    void parseBody();
public:
    RequestContext(WiFiClient* client);
    RequestContext(const RequestContext &request);
    ~RequestContext();

    WiFiClient* getClient();

    char *getBody();
    string getHeader(string key);
    string getQueryParameter(string key);
    const char* getPath();
    HttpMethod getMethod();
    bool isWebsocket();
};
