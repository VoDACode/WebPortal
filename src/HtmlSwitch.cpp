#include "./HtmlSwitch.h"

HtmlSwitch::HtmlSwitch(const char *id, bool value) : CANVASHtmlNode(id)
{
    this->_value = value;
    this->build();
}

HtmlSwitch::HtmlSwitch(bool value) : CANVASHtmlNode()
{
    this->_value = value;
    this->build();
}

HtmlSwitch::~HtmlSwitch()
{
}

void HtmlSwitch::build()
{
    this->addClass("switch");
    if (this->_value)
        this->addClass("active");
    this->on("$beforeEmitEventCallback", [](const char *eventName, void *node, vector<void *> *context, void *data)
             {
        if(strcmp(eventName, "change") == 0 || strcmp(eventName, "switched") == 0){
            HtmlSwitch *switchNode = (HtmlSwitch *)node;
            switchNode->setValue(!switchNode->_value);
        } });
}

void HtmlSwitch::setValue(bool value)
{
    this->_value = value;
    if (this->_value){
        this->addClass("active");
    }
    else
    {
        this->removeClass("active");
    }
}

void HtmlSwitch::onSwitched(event_callback_t *callback, vector<void *> *context)
{
    this->on("switched", callback, context);
}

bool HtmlSwitch::getValue()
{
    return this->_value;
}

const char *HtmlSwitch::getJs()
{
    return "(()=>{let t={none:'transparent',background:{active:'#2ecc71',inactive:'#ccc'},switch:'#FFFFFF'},e='active';document.querySelectorAll('.switch').forEach(i=>{if(void 0===i.getContext){console.error('This object does not canvas'),console.error(i);return}let c=i.getContext('2d'),n=i.width,a=i.height,r=!1,s=!1,l=0,o=!1,_=!1,d={x:76,y:a/2};function h(){if(!1==s&&l>1){requestAnimationFrame(h);return}c.clearRect(0,0,n,a),c.strokeStyle=t.none,c.fillStyle=r?t.background.active:t.background.inactive,c.beginPath(),c.roundRect(1,1,n-2,a-2,140),c.stroke(),c.beginPath(),c.roundRect(11,11,n-2-20,a-2-20,120),c.stroke(),c.fill(),c.fillStyle=t.switch,c.beginPath(),c.arc(d.x,d.y,a-1-95,0,2*Math.PI),c.stroke(),c.fill(),s&&(r?d.x>79?d.x-=8:(r=!1,_&&i.classList.remove(e),s=!1,o=!1):d.x<n-1-70-8-1.5?d.x+=6.5:(r=!0,_&&i.classList.add(e),s=!1,o=!1)),l<1&&l++,requestAnimationFrame(h)}h(),i.addEventListener('click',t=>{o||(o=!0,i.dispatchEvent(new Event('switched'+(r?'On':'Off'))),i.dispatchEvent(new Event('switched')),i.dispatchEvent(new Event('change')))}),window.addEventListener('load',()=>{i.classList.contains(e)&&(_=!0,s=!0)});new MutationObserver(t=>{t.forEach(t=>{if('class'==t.attributeName){if(_){_=!1;return}s=!0}})}).observe(i,{attributes:!0,attributeFilter:['class'],attributeOldValue:!0})})})();";
}

const char *HtmlSwitch::getCss()
{
    return ".switch{--switch-width:35px;--switch-height:20px;--scale:2.5;width:calc(var(--switch-width) * var(--scale));height:calc(var(--switch-height) * var(--scale));cursor:pointer;border-radius:140px}";
}