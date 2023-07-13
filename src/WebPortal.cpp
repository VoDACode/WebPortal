#include "WebPortal.h"

WebPortal::WebPortal(int port)
{
    this->server = new WiFiServer(port);
    this->options.title = "WebPortal - dev by VoDACode";
    this->options.customCss = "";
    this->options.customJs = "";
}

WebPortal::~WebPortal()
{
}

void WebPortal::handle()
{
    WiFiClient client = server->accept();
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
        Serial.println("Client disconnected");
    }
}

void WebPortal::clientHandler(HttpClient* client){
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

    client->send("</head><body>");
    // body

    // content
    HtmlNode* content;
    if(this->page != nullptr){
        content = this->page;
    }else{
        content = this->pageNoContent();
    }
    char* tmp = content->toString();
    client->send(tmp);
    delete[] tmp;
    delete content;

    // custom js
    if (this->options.customJs != NULL)
    {
        client->send("<script>");
        client->send(this->options.customJs.c_str());
        client->send("</script>");
    }
    client->send("</body></html>");
    client->end();
}

void WebPortal::begin()
{
    this->server->begin();
}

void WebPortal::setCustomCss(const char* css)
{
    this->options.customCss = css;
}

void WebPortal::setCustomJs(const char* js)
{
    this->options.customJs = js;
}

void WebPortal::setTitle(const char* title)
{
    this->options.title = title;
}

void WebPortal::setPage(HtmlNode* page)
{
    this->page = page;
}

HtmlNode* WebPortal::pageNoContent(){
    HtmlNode* content = new HtmlNode("h1", false);
    HtmlBox* box = new HtmlBox("", false, content, false);

    content->setInnerText("No content");
    box->addChild(content);

    return box;
}