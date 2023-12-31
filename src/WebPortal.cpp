#include "WebPortal.h"

WebPortal::WebPortal(int port)
{
    LittleFS.begin();
    this->httpServer = new WebServer(port);
    this->options.title = "WebPortal - dev by VoDACode";
    this->options.customCss = "";
    this->options.customJs = "";

    HtmlNode *content = new HtmlNode("h1", false);
    HtmlBox *box = new HtmlBox("", false, content, false);

    content->setInnerText("No content");
    box->buildElement();
    this->pageNoContent = box;
    this->page = this->pageNoContent;

    this->httpServer->on(
        "/",
        HttpMethod::GET,
        [this](HttpContext &context)
        {
            this->handleIndex(&context);
        });

    this->httpServer->on(
        "/config",
        HttpMethod::GET,
        {[this](HttpContext &context)
         {
             this->httpServer->requestAuthentication("admin", "admin");
         },
         [this](HttpContext &context)
         {
             this->handleConfig(&context);
         }});

    this->httpServer->onWs(
        "/event",
        [this](HttpContext &context)
        {
            WsRequest *request = parseWsRequest(context.getWebSocket()->getMessage());
            // Serial.printf("Event: %s, app-id: %d, data: %s\n", request->event, request->appId, request->data);

            HtmlNodeContainer::getInstance()->getNode(request->appId)->emit(request->event, (void *)request->data);
            // Serial.println("Event handled");
            delete request;
        });
}

WebPortal::~WebPortal()
{
    LittleFS.end();
    delete this->httpServer;
}

void WebPortal::handle()
{
    this->httpServer->handle();

    char *response = HtmlNodeContainer::getInstance()->handleUpdatedNodes();

    if (response != NULL)
    {
        this->httpServer->broadcast(response);
        delete[] response;
    }
    if (this->otaEnabled)
    {
        ArduinoOTA.handle();
    }
}

void WebPortal::handleIndex(HttpContext *context)
{
    auto client = context->getResponse();
    client->setCode(200);
    client->send("<!DOCTYPE html><html><head>");
    // header
    // default css

#ifdef DYNAMIC_DEVELOPMENT
    client->send(WebPortal::DEFAULT_CSS);
#else
    client->send("<style>");
    client->sendFileContent((WEB_PORTAL_PATH + String("default.css")).c_str());
    client->send("</style>");
#endif

    // meta
    client->send("<meta charset=\"utf-8\">");
    client->send("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

    // title
    client->send("<title>");
    client->send(this->options.title.c_str());
    client->send("</title>");

    // custom css
    if (this->options.customCss != nullptr)
    {
        client->send("<style>");
        client->send(this->options.customCss.c_str());
        client->send("</style>");
    }
    client->send("<style>");
    this->page->sendCssTo(client);
    client->send("</style>");

// default js
#ifdef DYNAMIC_DEVELOPMENT
    client->send(WebPortal::WS_JS);
#else
    client->send("<script>");
    client->sendFileContent((WEB_PORTAL_PATH + String("ws.js")).c_str());
    client->send("</script>");
#endif

    client->send("</head><body>");
    // body

    // set theme
    client->sendFileContent((WEB_PORTAL_PATH + String("theme_part_1.html")).c_str());
    client->send(this->options.pageTitle.c_str());
    client->sendFileContent((WEB_PORTAL_PATH + String("theme_part_2.html")).c_str());

    // content
    this->page->sendHtmlTo(client);

    // custom js
    if (this->options.customJs != NULL)
    {
        client->send("<script>");
        client->send(this->options.customJs.c_str());
        client->send("</script>");
    }

    // after load js
    client->send("<script>");
    this->page->sendJsTo(client);
    client->send("</script>");

#ifdef DYNAMIC_DEVELOPMENT
    client->send(WebPortal::THEME_JS);
#else
    client->send("<script>");
    client->sendFileContent((WEB_PORTAL_PATH + String("theme.js")).c_str());
    client->send("</script>");
#endif

    client->send("</body></html>");
}

void WebPortal::handleConfig(HttpContext *context)
{
    auto response = context->getResponse();

    response->setCode(200);
    response->setHeader("Content-Type", "application/json");
    response->send("{ \"version\": ");
    response->send(String(this->getVersion()).c_str());
    response->send(", \"title\": \"");
    response->send(this->options.title.c_str());
    response->send("\", \"page-title\": \"");
    response->send(this->options.pageTitle.c_str());
    response->send("\", \"ota\": { \"enabled\": ");
    response->send(this->otaEnabled ? "true" : "false");
    response->send(" } }");
}

void WebPortal::begin()
{
    this->httpServer->begin();
    if (this->page == nullptr)
        this->page = this->pageNoContent;
    if (this->options.pageTitle == "")
        this->options.pageTitle = String("WebPortal - ") + String(ESP.getChipId());
}

void WebPortal::setCustomCss(const char *css)
{
    this->options.customCss = css;
}

void WebPortal::setCustomJs(const char *js)
{
    this->options.customJs = js;
}

void WebPortal::setTitle(const char *title)
{
    this->options.title = title;
}

void WebPortal::setPageTitle(const char *pageTitle)
{
    if (pageTitle == nullptr)
        this->options.pageTitle = String("WebPortal - ") + String(ESP.getChipId());
    else
        this->options.pageTitle = pageTitle;
}

void WebPortal::setPage(HtmlNode *page)
{
    if (page == nullptr)
        this->page = this->pageNoContent;
    else
        this->page = page;
}

void WebPortal::intToBytes(int value, char *bytes)
{
    // number - 3 bytes (0 - signed, 1 - unsigned)
    bytes[0] = value >= 0 ? 1 : 0;
    value = value > 0 ? value : -value;
    bytes[1] = value & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
}

int WebPortal::bytesToInt(char *bytes)
{
    int value = 0;
    value |= bytes[2] << 8;
    value |= bytes[1];
    if (bytes[0] < 0)
    {
        value = -value;
    }
    return value;
}

char *WebPortal::request(const char *event, int appId, const char *data)
{
    int eventLength = strlen(event);
    int dataLength = strlen(data);
    int totalLength = 3 + 3 + eventLength + 3 + dataLength;
    char *bytes = new char[totalLength];
    int index = 0;

    // create header

    intToBytes(eventLength, bytes + index);
    index += 3;
    intToBytes(dataLength, bytes + index);
    index += 3;

    // create body
    memcpy(bytes + index, event, eventLength);
    index += eventLength;
    intToBytes(appId, bytes + index);
    index += 3;
    memcpy(bytes + index, data, dataLength);
    index += dataLength;

    return bytes;
}

WsRequest *WebPortal::parseWsRequest(char *bytes)
{
    WsRequest *request = new WsRequest();
    int index = 0;

    // parse header
    int eventLength = bytesToInt(bytes + index);
    index += 3;
    int dataLength = bytesToInt(bytes + index);
    index += 3;

    // parse body
    request->event = new char[eventLength + 1];
    memcpy(request->event, bytes + index, eventLength);
    request->event[eventLength] = '\0';
    index += eventLength;
    request->appId = bytesToInt(bytes + index);
    index += 3;
    request->data = new char[dataLength + 1];
    memcpy(request->data, bytes + index, dataLength);
    request->data[dataLength] = '\0';
    index += dataLength;

    return request;
}

void WebPortal::setupOTA(String password)
{
    this->setupOTA(password, 8266);
}

void WebPortal::setupOTA(String password, int port)
{
    this->otaEnabled = true;
    ArduinoOTA.setHostname((String("webportal-") + String(ESP.getChipId())).c_str());
    ArduinoOTA.setPassword(password.c_str());
    ArduinoOTA.setPort(port);
    ArduinoOTA.begin();
}

int WebPortal::getVersion()
{
    return 131;
}