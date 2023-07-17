#pragma once
#include <vector>
#include "html_templates/HtmlBox.h"
#include "HtmlNode.h"
#include "HttpClient.h"
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

using namespace std;

struct WebPortalOptions
{
	String title;
	String customCss;
	String customJs;
	String pageTitle;
};

class WebPortal
{
private:
	WebPortalOptions options;
	WiFiServer* httpServer;
	WebSocketsServer* wsServer;
	HtmlNode* page;
	HtmlNode* pageNoContent;
	void clientHandler(HttpClient* client);
	static void wsHandler(WebPortal *portal, uint8_t num, WStype_t type, uint8_t * payload, size_t length);

public:
	static constexpr const char* THEME_HTML_PART_1 = "<div class='header'><div class='header-title'><h1 id='title-text'>";
	static constexpr const char* THEME_HTML_PART_2 = "</h1></div><div class='theme'><div><span>Dark theme</span></div><div class='switch-old'><input type='checkbox' id='theme-switcher'><label for='theme-switcher'></label></div></div></div>";
	static constexpr const char* THEME_JS = "(()=>{let e=document.getElementById('theme-switcher');!function(){let t=localStorage.getItem('theme');console.log(t),t?(e.checked='dark'==t,document.body.classList.add(t)):document.body.classList.add('light')}(),e.addEventListener('click',e=>{e.target.checked?(document.body.classList.add('dark'),document.body.classList.remove('light'),localStorage.setItem('theme','dark')):(document.body.classList.add('light'),document.body.classList.remove('dark'),localStorage.setItem('theme','light'))})})();";

	static constexpr const char* WS_JS = "(()=>{let t=new WebSocket(`ws://${location.host}:81/ws`),e=0,n=1e4,i=null;function r(t){return document.querySelector(`[app-id='${t}']`)}function o(t){let e=r(t);e&&e.remove()}function a(t){let e=r(t['app-id']);if(e){for(;e.attributes.length>0;)e.removeAttribute(e.attributes[0].name);for(let n in t.attributes)e.setAttribute(n,t.attributes[n]);if(t.innerText&&(e.innerText=t.innerText),t.tag&&t.tag!==e.tagName){let i=document.createElement(t.tag);for(let o in e.attributes)i.setAttribute(o,e.attributes[o]);for(i.innerText=e.innerText;e.firstChild;)i.appendChild(e.firstChild);e.replaceWith(i)}}}function l(){!(Date.now()-e<n)&&(e=Date.now(),t.send(JSON.stringify({event:'ping'})))}function s(n,i){r(n).addEventListener(i,r=>{e=Date.now(),t.send(JSON.stringify({event:i,'app-id':n,data:r.target.value}))})}t.onopen=()=>{console.log('Connected'),i=setInterval(l,n)},t.onmessage=t=>{let e=JSON.parse(t.data);console.log(e);let n=e.data;if(200===e.status){for(let i of n.update)a(i);for(let r of n.remove)o(r)}},t.onclose=()=>{console.log('Disconnected')},t.onerror=t=>{console.log(t)},window.app={addEvent:s}})();";
	static constexpr const char* DEFAULT_CSS = ".dark,.light{--input-border:#A0A0A0;--input-placeholder:#A0A0A0;--input-error:#FF3B30}.header,.header>.theme{display:flex;padding:10px}input,select{font-size:1.5em;color:var(--input-text);transition:background-color .4s ease-in-out,color .4s ease-in-out;min-width:var(--input-min-width)}button,td{text-align:center}*{margin:0;padding:0;box-sizing:border-box;font-family:var(--main-font)}:root{--background-color:#f5f5f5;--white:#fff;--main-font:'Roboto',sans-serif;--border-color:#e0e0e0;--input-min-width:270px}.light{background-color:var(--background-color);--input-background:#FFFFFF;--input-text:#3f3f3f;--input-focus:#007AFF;--box-background-color:#c0c0c0}.dark{background-color:#3a1078;color:#fff;--input-background:#4E31AA;--input-text:#FFFFFF;--input-focus:#1A73E8;--box-background-color:#4E31AA}.header,.model{background-color:var(--box-background-color)}.box,.model{color:var(--text-color)}.header{z-index:100;position:fixed;flex-direction:row;justify-content:space-between;align-items:center;border-bottom:1px solid var(--border-color);width:100%;margin-top:0;height:50px;top:0}.model,input,select{padding:5px}.box,.input-box{margin-top:10px;margin-bottom:10px}main{position:absolute;display:flex;flex-direction:column;justify-content:center;align-items:center;width:100%;top:50px}select{border:1px solid var(--input-border);border-radius:5px;background-color:var(--input-background);margin-left:5px;margin-right:5px}input:focus,select:focus{outline:0;border-color:var(--input-focus);box-shadow:0 0 0 2px var(--input-focus)}input.error,select.error{border-color:var(--input-error)}select::placeholder{color:var(--input-placeholder)}select>option{color:var(--input-text)}select>option:active,select>option:focus,select>option:hover{background-color:var(--input-focus)}input,select>option:disabled,select>option:disabled:active,select>option:disabled:focus,select>option:disabled:hover{background-color:var(--input-background)}.inline{display:flex;flex-direction:row!important;justify-content:center!important;align-items:center!important}@media screen and (max-width:700px){.inline{flex-direction:column!important}}.header>.theme{flex-direction:row;justify-content:center;align-items:center}.header>.theme>div{margin:0 10px}input{border:1px solid var(--input-border);border-radius:5px}.box:not(.sub)>div>.box-content,.box:not(.sub)>div>.box-footer,.box:not(.sub)>div>.box-header,button{padding:10px}input::placeholder{color:var(--input-placeholder)}.switch-old{position:relative;display:inline-block;width:60px;height:34px}.switch-old input{display:none}.input-box,.line-box,.model,.model-overlay.active{display:flex}.switch-old label{position:absolute;top:0;left:0;right:0;bottom:0;background-color:#ccc;border-radius:34px;cursor:pointer;transition:background-color .4s ease-in-out;margin:0!important}.switch-old label::before{position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;background-color:var(--white);border-radius:50%;transition:transform .4s ease-in-out}.switch-old input:checked+label{background-color:#2ecc71}.switch-old input:checked+label::before{transform:translateX(26px)}input[type=radio]{-webkit-appearance:none;-moz-appearance:none;appearance:none;width:20px;height:20px;background:var(--background-color);border-radius:50%;position:relative;cursor:pointer}input[type=radio]:before{content:'';position:absolute;width:10px;height:10px;background:var(--white);border-radius:50%;top:5px;left:5px;transition:.5s;opacity:0}input[type=radio]:checked:before,input[type=range]:hover{opacity:1}input[type=radio]:checked{background:#4caf50}input[type=range]{width:100%;height:5px;background:var(--background-color);outline:0;opacity:.7;-webkit-transition:.2s;transition:opacity .2s}input[type=range]::-moz-range-thumb,input[type=range]::-ms-thumb,input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;appearance:none;width:25px;height:25px;border-radius:50%;background:#4caf50;cursor:pointer}input[type=range]::-moz-range-track,input[type=range]::-ms-track,input[type=range]::-webkit-slider-runnable-track{width:100%;height:5px;cursor:pointer;animation-duration:.2s;box-shadow:1px 1px 1px #000;background:#3071a9;border-radius:25px;border:0 solid #000}.input-box{flex-direction:column}.line-box{flex-direction:row;justify-content:space-between;align-items:center;max-width:fit-content}.box:not(.sub)>div,.model{max-width:750px;min-width:350px}.input-box label{margin-bottom:5px}.input-box.line-box label{margin-right:5px}button{border:none;border-radius:5px;font-size:16px;font-weight:700;cursor:pointer;transition:background-color .4s ease-in-out,color .4s ease-in-out}button.accept{background-color:#2ecc71;color:#fff}button.reject{background-color:#e74c3c;color:#fff}button.accept:not(:disabled):hover,button.reject:not(:disabled):hover{opacity:.8}button.accept:focus,button.reject:focus{outline:0;box-shadow:0 0 0 2px #3498db}button:disabled{opacity:.5;cursor:not-allowed}button.accept:not(:disabled):hover{background-color:#27ae60}button.reject:not(:disabled):hover{background-color:#c0392b}.light button.accept:focus,.light button.reject:focus{box-shadow:0 0 0 2px #bbdefb}.dark button.accept:focus,.dark button.reject:focus{box-shadow:0 0 0 2px #5dade2}.box,.box>div{display:flex;flex-direction:column;justify-content:center;align-items:center}.box{width:100%;height:100%}.box:not(.sub)>div{border:1px solid var(--border-color);border-radius:5px}.box>div>.box-footer,.box>div>.box-header{flex-direction:row;justify-content:space-between;align-items:center}.box>div>.box-content,.model{flex-direction:column;margin:10px}.box>div>.box-content{justify-content:center;align-items:center}.model{justify-content:center;align-items:center;border-radius:5px}.model-overlay{position:fixed;display:none;flex-direction:column;justify-content:center;align-items:center;top:0;left:0;width:100%;height:100%;background-color:rgba(0,0,0,.5);z-index:999}.model>.model-footer,.model>.model-header{flex-direction:row;justify-content:space-between;align-items:center;padding:10px}.model>.model-content{flex-direction:column;justify-content:center;align-items:center}table{border-collapse:separate;border-spacing:0 1em;width:100%}";
	WebPortal(int port = 80);
	~WebPortal();

	void begin();
	void handle();
	void setTitle(const char* title);
	void setCustomCss(const char* customCss);
	void setCustomJs(const char* customJs);
	void setPageTitle(const char* pageTitle);
	void setPage(HtmlNode* page);
};