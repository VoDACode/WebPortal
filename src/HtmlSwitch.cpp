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
    this->hasCssCode = true;
    this->hasJsCode = true;
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

const char *HtmlSwitch::componentName()
{
    return "switch";
}