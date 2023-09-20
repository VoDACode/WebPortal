#include "RequestContext.h"

RequestContext::RequestContext(WiFiClient *client)
{
    this->client = client;
    this->websocket = false;

    string header = this->readHeader();

    int firstLineEnd = this->parsePathAndMethod(header);
    // this->parseQueryParameters();
    this->parseHeader(header, firstLineEnd);
}

RequestContext::RequestContext(const RequestContext &request)
{
    this->client = request.client;
    this->headers = request.headers;
    this->body = new char[strlen(request.body) + 1];
    strcpy(this->body, request.body);
    this->queryParameters = request.queryParameters;
    this->path = new char[strlen(request.path) + 1];
    strcpy(this->path, request.path);
    this->method = request.method;
    this->websocket = request.websocket;
}

RequestContext::~RequestContext()
{
    delete this->path;
    delete this->body;
}

WiFiClient *RequestContext::getClient()
{
    return this->client;
}

char *RequestContext::getBody()
{
    if (this->body == nullptr)
    {
        this->parseBody();
    }
    return this->body;
}

string RequestContext::getHeader(string key)
{
    if (this->headers.find(key) == this->headers.end())
    {
        return "";
    }
    return this->headers[key];
}

string RequestContext::getQueryParameter(string key)
{
    return this->queryParameters[key];
}

const char *RequestContext::getPath()
{
    return this->path;
}

HttpMethod RequestContext::getMethod()
{
    return this->method;
}

bool RequestContext::isWebsocket()
{
    return this->websocket;
}

int RequestContext::parsePathAndMethod(string header)
{
    int firstLineEnd = header.find("\r\n");
    string firstLine = header.substr(0, firstLineEnd);
    int firstSpace = firstLine.find(" ");
    string method = firstLine.substr(0, firstSpace);
    string path = firstLine.substr(firstSpace + 1);
    int pathEnd = path.find(" ");
    path = path.substr(0, pathEnd);

    this->path = new char[path.length() + 1];
    strcpy(this->path, path.c_str());

    if (method == "GET")
    {
        this->method = HttpMethod::GET;
    }
    else if (method == "POST")
    {
        this->method = HttpMethod::POST;
    }
    else if (method == "PUT")
    {
        this->method = HttpMethod::PUT;
    }
    else if (method == "DELETE")
    {
        this->method = HttpMethod::DELETE;
    }
    else if (method == "PATCH")
    {
        this->method = HttpMethod::PATCH;
    }
    else if (method == "OPTIONS")
    {
        this->method = HttpMethod::OPTIONS;
    }
    else if (method == "HEAD")
    {
        this->method = HttpMethod::HEAD;
    }
    else if (method == "TRACE")
    {
        this->method = HttpMethod::TRACE;
    }
    else if (method == "CONNECT")
    {
        this->method = HttpMethod::CONNECT;
    }
    return firstLineEnd;
}

void RequestContext::parseQueryParameters()
{
    string path = this->path;
    int queryStart = path.find("?");
    if (queryStart != -1)
    {
        string query = path.substr(queryStart + 1);
        int queryEnd = query.find(" ");
        query = query.substr(0, queryEnd);
        int queryParameterStart = 0;
        while (queryParameterStart < query.length())
        {
            int queryParameterEnd = query.find("&", queryParameterStart);
            if (queryParameterEnd == -1)
            {
                queryParameterEnd = query.length();
            }
            string queryParameter = query.substr(queryParameterStart, queryParameterEnd - queryParameterStart);
            int queryParameterEqual = queryParameter.find("=");
            string key = queryParameter.substr(0, queryParameterEqual);
            string value = queryParameter.substr(queryParameterEqual + 1);
            this->queryParameters[key] = value;
            queryParameterStart = queryParameterEnd + 1;
        }
    }
}

void RequestContext::parseHeader(string header, int firstLineEnd)
{
    int headerEnd = header.find("\r\n\r\n");
    int headerStart = firstLineEnd + 2;
    while (headerStart < headerEnd)
    {
        int headerLineEnd = header.find("\r\n", headerStart);
        string headerLine = header.substr(headerStart, headerLineEnd - headerStart);
        int headerLineColon = headerLine.find(": ");
        string key = headerLine.substr(0, headerLineColon);
        string value = headerLine.substr(headerLineColon + 2);
        this->headers[key] = value;
        headerStart = headerLineEnd + 2;
        if (key == "Upgrade" && value == "websocket")
        {
            Serial.printf("[%s] Websocket\n", this->path);
            this->websocket = true;
        }
    }
}

void RequestContext::parseBody()
{
    int contentLength = atoi(this->headers["Content-Length"].c_str());
    this->body = new char[contentLength + 1];
    int bodyLength = 0;
    while (this->client->connected())
    {
        if (this->client->available())
        {
            char c = this->client->read();
            this->body[bodyLength++] = c;
            if (bodyLength == contentLength)
            {
                break;
            }
        }
    }
    this->body[bodyLength] = '\0';
}

string RequestContext::readHeader()
{
    string header = "";
    header.reserve(1024);
    while (this->client->connected())
    {
        if (this->client->available())
        {
            char c = this->client->read();
            header += c;
            if (header.length() > 4)
            {
                if (header.substr(header.length() - 4) == "\r\n\r\n")
                {
                    break;
                }
            }
        }
    }

    return header;
}
