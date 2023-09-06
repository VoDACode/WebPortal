#include "WebPortal.h"

WebPortal::WebPortal(int port)
{
    this->httpServer = new WiFiServer(port);
    this->wsServer = new WebSocketsServer(port + 1);
    this->options.title = "WebPortal - dev by VoDACode";
    this->options.customCss = "";
    this->options.customJs = "";

    HtmlNode *content = new HtmlNode("h1", false);
    HtmlBox *box = new HtmlBox("", false, content, false);

    content->setInnerText("No content");
    box->buildElement();
    this->pageNoContent = box;
    this->page = this->pageNoContent;
}

WebPortal::~WebPortal()
{
    delete this->httpServer;
    delete this->wsServer;
}

void WebPortal::handle()
{
    // Handle ws api request here. Path: /ws
    // After handle api request, return JSON response.
    /*
        Example response:
        {
            "status": 200,
            "message": "OK",
            "data": {
                "update": [
                    {
                        "app-id": 1,
                        "attributes": {
                            "class": "label label-success"
                        },
                        "innerText": "Click: 1"
                    }
                ],
                "remove": [],   // list of app-id
                "add": [
                    {
                        "parent": 1,
                        "tag": "div",
                        "app-id": 2,
                        "attributes": {
                            "class": "label label-success"
                        },
                        "innerText": "New label"
                    }
                ]
            }
        }
    */
    this->wsServer->loop();

    char *response = HtmlNodeContainer::getInstance()->handleUpdatedNodes();
    if (response != NULL)
    {
        this->wsServer->broadcastTXT(response, strlen(response));
        delete[] response;
    }

    this->wsServer->loop();
    WiFiClient client = httpServer->accept();
    if (client)
    {
        while (client.connected())
        {
            if (client.available())
            {
                HttpClient httpClient(&client);
                this->clientHandler(&httpClient);
                break;
            }
        }
    }
}

void WebPortal::wsHandler(WebPortal *portal, uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED: // if the websocket is disconnected
        Serial.printf("[%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = portal->wsServer->remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        break;
    }
    case WStype_TEXT:
    {
        // parse json
        StaticJsonDocument<200> doc;
        deserializeJson(doc, payload);
        JsonObject data = doc.as<JsonObject>();
        String event = data["event"].as<String>();

        if (event == "ping")
        {
            portal->wsServer->sendTXT(num, "{\"status\": 204, \"message\": \"OK\"}");
            break;
        }

        unsigned long long appId = data["app-id"].as<unsigned long long>();
        String dataStr = data["data"].as<String>();
        // Serial.printf("Event: %s, app-id: %llu, data: %s\n", event.c_str(), appId, dataStr.c_str());
        HtmlNodeContainer::getInstance()->getNode(appId)->emit(event.c_str(), (void *)dataStr.c_str());
        break;
    }
    default:
        break;
    }
}

void WebPortal::clientHandler(HttpClient *client)
{
    client->status(200);
    client->send("<!DOCTYPE html><html><head>");
    // header
    // default css
    client->send("<style>");
    client->send(WebPortal::DEFAULT_CSS);
    client->send("</style>");

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
    client->send("<script>");
    client->send(WebPortal::WS_JS);
    client->send("</script>");

    client->send("</head><body>");
    // body

    // set theme
    client->send(WebPortal::THEME_HTML_PART_1);
    client->send(this->options.pageTitle.c_str());
    client->send(WebPortal::THEME_HTML_PART_2);

    // content
    char *htmlContent = this->page->toString();
    client->send(htmlContent);
    delete[] htmlContent;

    // custom js
    if (this->options.customJs != NULL)
    {
        client->send("<script>");
        client->send(this->options.customJs.c_str());
        client->send("</script>");
    }

    // after load js
    client->send("<script>");
    char *afterLoadJs = this->page->generateJsCode();
    client->send(afterLoadJs);
    delete[] afterLoadJs;
    client->send("</script>");

    client->send("<script>");
    client->send(WebPortal::THEME_JS);
    client->send("</script>");

    client->send("</body></html>");
    client->end();
}

void WebPortal::begin()
{
    this->wsServer->begin();
    this->wsServer->onEvent([this](uint8_t num, WStype_t type, uint8_t *payload, size_t length)
                            { WebPortal::wsHandler(this, num, type, payload, length); });
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