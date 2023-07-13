#include "HttpClient.h"

HttpClient::HttpClient(WiFiClient *client)
{
    this->client = client;
    // parse header to dictionary

    this->parseHeader();
}

HttpClient::~HttpClient()
{
}

void HttpClient::parseHeader()
{
    string header = "";
    header.reserve(1024);
    while (this->client->connected())
    {
        if (this->client->available())
        {
            header.append(this->client->readStringUntil('\r').c_str());
            break;
        }
    }

    // parse header to dictionary like key: value
    // parse first line
    int firstLineEnd = header.find("\r\n");
    string firstLine = header.substr(0, firstLineEnd);
    int firstSpace = firstLine.find(" ");
    this->method = firstLine.substr(0, firstSpace);
    int secondSpace = firstLine.find(" ", firstSpace + 1);
    this->path = firstLine.substr(firstSpace + 1, secondSpace - firstSpace - 1);

    // parse other lines
    std::size_t lineEnd = header.find("\r\n", firstLineEnd + 2);
    while (lineEnd != string::npos)
    {
        string line = header.substr(firstLineEnd + 2, lineEnd - firstLineEnd - 2);
        int colon = line.find(": ");
        string key = line.substr(0, colon);
        if (key == "")
        {
            break;
        }
        string value = line.substr(colon + 2, lineEnd - colon - 2);
        if (this->headers.find(key) != this->headers.end())
        {
            this->headers.erase(key);
        }
        this->headers.insert(std::pair<string, string>(key, value));
        firstLineEnd = lineEnd;
        lineEnd = header.find("\r\n", firstLineEnd + 2);
    }
}

string HttpClient::getMethod()
{
    return this->method;
}

string HttpClient::getPath()
{
    return this->path;
}

string HttpClient::getHeader(string key)
{
    if (this->headers.find(key) == this->headers.end())
    {
        return "";
    }
    return this->headers[key];
}

HttpClient &HttpClient::send(string data)
{
    if (!this->canSendContent)
    {
        this->canSendContent = true;
        this->client->print("\r\n");
    }
    this->client->print(data.c_str());
    return *this;
}

HttpClient &HttpClient::send(const char *data)
{
    if (!this->canSendContent)
    {
        this->canSendContent = true;
        this->client->print("\r\n");
    }

    // send data part by part to avoid memory overflow
    int dataLength = strlen(data);
    int partLength = 1024;
    int partCount = dataLength / partLength;
    int lastPartLength = dataLength % partLength;

    for (int i = 0; i < partCount; i++)
    {
        this->client->write(data + i * partLength, partLength);
    }
    this->client->write(data + partCount * partLength, lastPartLength);
    return *this;
}

HttpClient &HttpClient::status(int code, string contentType)
{
    this->client->print("HTTP/1.1 ");
    this->client->print(code);
    this->client->print(" ");
    switch (code)
    {
    case 200:
        this->client->print("OK");
        break;
    case 404:
        this->client->print("Not Found");
        break;
    default:
        break;
    }
    this->client->print("\r\n");
    this->client->print("Content-Type: ");
    this->client->print(contentType.c_str());
    this->client->print("\r\n");
    this->client->print("Connection: close\r\n");
    return *this;
}

HttpClient &HttpClient::header(string key, string value)
{
    this->client->print(key.c_str());
    this->client->print(": ");
    this->client->print(value.c_str());
    this->client->print("\r\n");
    return *this;
}

HttpClient &HttpClient::end()
{
    this->client->print("\r\n");
    while (client->available())
    {
        client->read();
    }
    this->client->stop();
    return *this;
}

bool HttpClient::isClientConnected()
{
    return this->client->connected();
}

bool HttpClient::isClientAvailable()
{
    return this->client->available();
}
