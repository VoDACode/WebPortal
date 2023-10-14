#pragma once
#include <vector>
#include "HtmlBox.h"
#include "HtmlNode.h"
#include <ESP8266WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>

#include "FS.h"
#include "LittleFS.h"

#include "config.h"

using namespace std;

struct WebPortalOptions
{
	String title;
	String customCss;
	String customJs;
	String pageTitle;
};

struct WsRequest
{
	char *event = NULL;
	int appId;
	char *data = NULL;

	~WsRequest()
	{
		delete[] this->event;
		delete[] this->data;
	}
};

class WebPortal
{
private:
	WebPortalOptions options;
	WebServer *httpServer;

	HtmlNode *page;
	HtmlNode *pageNoContent;
	void handleIndex(HttpContext *client);
	void handleConfig(HttpContext *client);

	bool otaEnabled = false;

protected:
	void intToBytes(int value, char *bytes);
	int bytesToInt(char *bytes);
	char *request(const char *event, int appId, const char *data);
	WsRequest *parseWsRequest(char *bytes);

public:
#ifdef DYNAMIC_DEVELOPMENT
	static constexpr const char *THEME_JS = "<script src='" DYNAMIC_THEME_JS "'></script>";
	static constexpr const char *DEFAULT_CSS = "<link rel='stylesheet' href='" DYNAMIC_DEFAULT_CSS "'>";
	static constexpr const char *WS_JS = "<script src='" DYNAMIC_WS_JS "'></script>";
#endif

	WebPortal(int port = 80);
	~WebPortal();

	void begin();
	void handle();

	void setTitle(const char *title);
	void setCustomCss(const char *customCss);
	void setCustomJs(const char *customJs);
	void setPageTitle(const char *pageTitle);
	void setPage(HtmlNode *page);

	void setupOTA(String password);
	void setupOTA(String password, int port);

	int getVersion();
};