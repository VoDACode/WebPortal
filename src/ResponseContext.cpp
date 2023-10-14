#include "ResponseContext.h"

ResponseContext::ResponseContext(WiFiClient *client)
{
    this->client = client;
}

ResponseContext::~ResponseContext()
{
}

WiFiClient* ResponseContext::getClient()
{
    return this->client;
}

void ResponseContext::setCode(int code)
{
    this->code = code;
}

void ResponseContext::setHeader(const char *key, const char *value)
{
    this->headers[key] = value;
}

void ResponseContext::send()
{
    this->send("", this->code);
}

void ResponseContext::send(const char *body)
{
    this->send(body, this->code);
}

void ResponseContext::send(const char *body, int code)
{
    if(step == 0){
        this->setCode(code);
        this->sendHeader();
        this->sendBody(body);
        step = 1;
    }
    else if(step == 1){
        this->sendBody(body);
    }
}

void ResponseContext::sendFileContent(const char *filename)
{
    this->sendFileContent(filename, this->code);
}

void ResponseContext::sendFileContent(const char *filename, int code)
{
    if(step == 0){
        this->setCode(code);
        this->sendHeader();
        this->sendBodyFromFile(filename);
        step = 1;
    }
    else if(step == 1){
        this->sendBodyFromFile(filename);
    }
}

void ResponseContext::sendHeader()
{
    this->client->print("HTTP/1.1 ");
    this->client->print(this->code);
    this->client->print(" ");
    if(this->code == 200){
        this->client->print("OK");
    }
    else if(this->code == 401){
        this->client->print("Unauthorized");
    }
    else if(this->code == 404){
        this->client->print("Not Found");
    }
    else if(this->code == 500){
        this->client->print("Internal Server Error");
    }
    else{
        this->client->print(this->code);
    }
    this->client->print("\r\n");
    for (auto it = this->headers.begin(); it != this->headers.end(); it++)
    {
        this->client->print(it->first);
        this->client->print(": ");
        this->client->print(it->second);
        this->client->print("\r\n");
    }

    this->client->print("Connection: close\r\n");

    this->client->print("\r\n");
}

void ResponseContext::sendBody(const char *body)
{
    this->client->print(body);
}

void ResponseContext::sendBodyFromFile(const char *filename){
    File file = LittleFS.open(filename, "r");
    if(!file)
    {
        Serial.printf("File '%s' not found\n", filename);
        return;
    }
    while(file.available()){
        char buffer[FILE_READ_BUFFER_SIZE + 1] = {0};
        int length = file.readBytes(buffer, FILE_READ_BUFFER_SIZE);
        buffer[length] = '\0';
        this->client->print(buffer);
        yield();
    }
    file.close();
}

void ResponseContext::end()
{
    this->ended = true;
    if(step == 0){
        this->sendHeader();
        step = 1;
    }

    this->client->print("\r\n");
    while (client->available())
    {
        client->read();
    }
    this->client->stop();
}

void ResponseContext::setEventSource()
{
    this->eventSource = true;
    this->client->print("HTTP/1.1 200 OK\r\n");
    this->client->print("Content-Type: text/event-stream\r\n");
    this->client->print("Cache-Control: no-cache\r\n");
    this->client->print("Connection: keep-alive\r\n");
    this->client->print("\r\n");
    Serial.println("Event source header sent");
}

void ResponseContext::sendEvent(const char *event, const char *data)
{
    this->client->print("event: ");
    this->client->print(event);
    this->client->print("\r\n");
    this->client->print("data: ");
    this->client->print(data);
    this->client->print("\r\n\r\n");
}

bool ResponseContext::isEventSource()
{
    return this->eventSource;
}

bool ResponseContext::isEnded()
{
    return this->ended;
}

void ResponseContext::unauthorized(const char *message)
{
    this->setCode(401);
    this->setHeader("WWW-Authenticate", "Basic realm=\"Secure Area\"");
    this->setHeader("Content-Type", "text/plain");
    this->send(message);
    this->end();
}