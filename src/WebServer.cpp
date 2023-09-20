#include "WebServer.h"

WebServer::WebServer(int port)
{
    this->server = new WiFiServer(port);
    this->port = port;
    this->clientCount = 0;
}

WebServer::~WebServer()
{
    delete this->server;

    for (int i = 0; i < this->callbacks.size(); i++)
    {
        delete this->callbacks[i];
    }

    for (int i = 0; i < this->clients->size(); i++)
    {
        delete this->clients->at(i);
    }
}

void WebServer::begin()
{
    this->server->begin();
}

void WebServer::handle()
{
    // Serial.printf("B: %d\n", ESP.getFreeHeap());
    if (this->server->status() == CLOSED)
    {
        Serial.println("Server closed. Reopening...");
        this->server->begin();
        Serial.println("Server reopened");
        // mem info
        Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    }

    WiFiClient client = this->server->accept();
    if (client && client.connected() && this->clientCount < MAX_CLIENTS)
    {
        this->clientCount++;
        HttpContext *context = new HttpContext(new WiFiClient(client));
        Serial.printf("---------New Client---------\n Client IP: %s\n", context->getClient()->remoteIP().toString().c_str());
        Serial.printf("[%s] [%d]\n", context->getRequest()->getPath(), context->getRequest()->getMethod());
        this->clients->push_back(context);
        this->handleClient(context);
    }
    this->manageClients();
    // Serial.printf("A: %d\n", ESP.getFreeHeap());
}

void WebServer::manageClients()
{
    for (int i = 0; i < this->clients->size(); i++)
    {
        HttpContext *client = this->clients->at(i);
        bool deleted = false;
        if (client == NULL)
        {
            this->clients->erase(this->clients->begin() + i);
            deleted = true;
        }
        if (client != nullptr && !client->getClient()->connected())
        {
            Serial.printf("[CLIENT-%d][%s][%d] Client disconnected\n", i, client->getRequest()->getPath(), client->getRequest()->getMethod());
            this->clientCount--;
            delete this->clients->at(i);
            this->clients->erase(this->clients->begin() + i);
            deleted = true;
        }
        if (deleted)
        {
            i--;
        }
        else if (client->isWebsocket())
        {
            handlingContext = client;
            for (int i = 0; i < this->callbacks.size(); i++)
            {
                if (this->callbacks[i]->websocket == true && client->getWebSocket()->handle())
                {
                    for(auto callback : this->callbacks[i]->callbacks){
                        callback(*client);
                        if(client->getResponse()->isEnded()){
                            break;
                        }
                    }
                    if (client->getResponse()->isEnded())
                    {
                        break;
                    }
                }
            }
        }
    }
    handlingContext = nullptr;
}

void WebServer::handleClient(HttpContext *context)
{
    bool found = false;
    for (int i = 0; i < this->callbacks.size(); i++)
    {
        if(context->getClient()->connected() == false){
            break;
        }
        if (this->matchPath(context->getRequest()->getPath(), this->callbacks[i]->path) && this->matchMethod(context->getRequest()->getMethod(), this->callbacks[i]->method))
        {
            if (this->callbacks[i]->websocket == false && context->isWebsocket() == false)
            {
                found = true;
                this->handlingContext = context;
                
                for(auto callback : this->callbacks[i]->callbacks){
                    callback(*context);
                    if(context->getResponse()->isEnded()){
                        break;
                    }
                }

                if (!context->getResponse()->isEventSource())
                {
                    context->getResponse()->end();
                }
                Serial.println("HTTP request handled");
            }
            else if (this->callbacks[i]->websocket == true && context->isWebsocket() == true)
            {
                found = true;
                this->handlingContext = context;
                context->getWebSocket()->sendHeader();
                context->getWebSocket()->setReadOnly(this->callbacks[i]->webSocketReadOnly);
                Serial.println("Websocket connected");
            }
        }
    }
    if (!found)
    {
        context->getResponse()->setHeader("Content-Type", "text/plain");
        context->getResponse()->setCode(404);
        context->getResponse()->send("Not found");
        context->getResponse()->end();
    }
    this->handlingContext = nullptr;
}

void WebServer::onWs(const char *path, HttpHandleCallback callback, bool webSocketReadOnly)
{
    HandleRecord *record = new HandleRecord();
    record->path = path;
    record->method = HttpMethod::GET;
    record->callbacks.push_back(callback);
    record->websocket = true;
    record->webSocketReadOnly = webSocketReadOnly;
    this->callbacks.push_back(record);
}

void WebServer::on(const char *path, HttpMethod method, HttpHandleCallback callback){
    this->on(path, method, {callback});
}

void WebServer::on(const char *path, HttpMethod method, std::initializer_list<HttpHandleCallback> callbacks)
{
    HandleRecord *record = new HandleRecord();
    record->path = path;
    record->method = method;

    for (auto callback : callbacks)
    {
        record->callbacks.push_back(callback);
    }

    record->websocket = false;
    record->webSocketReadOnly = false;
    this->callbacks.push_back(record);
}

void WebServer::requestAuthentication(const char* username, const char* password)
{
    HttpContext *context = this->handlingContext;
    
    String auth = context->getRequest()->getHeader("Authorization").c_str();
    Serial.printf("Authorization: %s\n", auth.c_str());

    if(auth == "" || (millis() - this->lastAuthorizationActivity) > SESSION_LIFETIME){
        context->getResponse()->unauthorized();
        return;
    }

    String realm = auth.substring(0, auth.indexOf(' '));

    if(realm == "Basic"){
        
        auth.replace("Basic ", "");
        if(rbase64.decode(auth) != RBASE64_STATUS_OK) {
            context->getResponse()->unauthorized();
            return;
        }

        String decoded = rbase64.result();

        int colonIndex = decoded.indexOf(':');
        String decodedUsername = decoded.substring(0, colonIndex);
        String decodedPassword = decoded.substring(colonIndex + 1);
        if(decodedUsername == username && decodedPassword == password){
            this->lastAuthorizationActivity = millis();
            return;
        }
    }
    context->getResponse()->unauthorized();
}

int WebServer::getClientCount()
{
    return this->clientCount;
}

bool WebServer::matchPath(const char *path, const char *pattern)
{
    int i = 0;
    int j = 0;
    while (path[i] != '\0' && pattern[j] != '\0')
    {
        if (path[i] == pattern[j])
        {
            i++;
            j++;
        }
        else if (pattern[j] == '*')
        {
            j++;
            while (path[i] != '\0')
            {
                i++;
            }
        }
        else
        {
            return false;
        }
    }
    return path[i] == '\0' && pattern[j] == '\0';
}

bool WebServer::matchMethod(HttpMethod method, HttpMethod pattern)
{
    return method == pattern || pattern == HttpMethod::ANY;
}

void WebServer::broadcast(String message)
{
    this->broadcast(message.c_str());
}

void WebServer::broadcast(const char *message)
{
    for (int i = 0; i < this->clients->size(); i++)
    {
        auto client = this->clients->at(i);
        if (client != nullptr && client->getClient()->connected() && client->isWebsocket() && !client->getWebSocket()->isReadOnly())
        {
            client->getWebSocket()->send(message);
        }
        else if (client != nullptr && client->getClient()->connected() && client->getResponse()->isEventSource())
        {
            Serial.println("[EVENT SOURCE] Sending message");
            client->getResponse()->sendEvent("message", message);
            Serial.println("[EVENT SOURCE] Ending");
        }
    }
}