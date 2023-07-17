#pragma once

#include <WiFiClient.h>
#include <map>
#include <string>

using namespace std;

class HttpClient
{
private:
    string method;
    string path;
    WiFiClient *client;
    std::map<string, string> headers;
    void parseHeader();

    bool canSendContent = false;

public:
    HttpClient(WiFiClient *client);
    ~HttpClient();
    string getMethod();
    string getPath();
    string getHeader(string key);

    /*
        The first you need set status code
        Then you can set headers
        Then you can send data

        Example:
        client.status(200).header("Content-Type", "text/html").send("<h1>Hello world</h1>").end();
        Client will receive:
        HTTP/1.1 200 OK
        Content-Type: text/html

        <h1>Hello world</h1>

        If you want to send data in chunks, you can use send() method
        Example:
        client.status(200).header("Content-Type", "text/html").send("<h1>Hello world</h1>").send("<h2>Second header</h2>").end();
        Client will receive:
        HTTP/1.1 200 OK
        Content-Type: text/html

        <h1>Hello world</h1><h2>Second header</h2>

    */

    HttpClient& status(int code, string contentType = "text/html");
    HttpClient& header(string key, string value);
    HttpClient& send(string data);
    HttpClient& send(const char* data);
    HttpClient& end();

    bool isClientConnected();
    bool isClientAvailable();
};
