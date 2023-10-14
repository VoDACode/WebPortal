#pragma once

//#define DYNAMIC_DEVELOPMENT 1
#ifdef DYNAMIC_DEVELOPMENT
#define DYNAMIC_WS_JS "http://10.0.1.20:3300/ws.js"
#define DYNAMIC_DEFAULT_CSS "http://10.0.1.20:3300/index.css"
#define DYNAMIC_THEME_JS "http://10.0.1.20:3300/theme.js"
#endif

#define MAX_CLIENTS 100
#define FILE_READ_BUFFER_SIZE 32
#define FORMAT_SPIFFS_IF_FAILED true

#define COMPONENTS_PATH "/www/components/"
#define JS_EXT ".js"
#define CSS_EXT ".css"