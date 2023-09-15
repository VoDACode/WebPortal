#include "HttpContext.h"

HttpContext::HttpContext(WiFiClient *client)
{
    this->request = new RequestContext(client);
    this->response = new ResponseContext(client);
    this->client = client;
    this->websocket = nullptr;
    this->handleRequest();
}

HttpContext::~HttpContext()
{
    delete this->request;
    delete this->response;
    if(this->websocket != nullptr)
        delete this->websocket;
    delete this->client;
}

void HttpContext::handleRequest()
{
    if (this->request->isWebsocket())
    {
        this->websocket = new WebSocketContext(this->client, this->request);
    }
}

RequestContext* HttpContext::getRequest()
{
    return this->request;
}

ResponseContext* HttpContext::getResponse()
{
    return this->response;
}

WebSocketContext* HttpContext::getWebSocket()
{
    return this->websocket;
}

bool HttpContext::isWebsocket()
{
    return this->getRequest()->isWebsocket();
}

WiFiClient *HttpContext::getClient()
{
    return this->client;
}