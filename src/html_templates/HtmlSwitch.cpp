#include "./HtmlSwitch.h"

HtmlSwitch::HtmlSwitch(const char *id, const char *name, bool value)
    : HtmlNode("div", false)
{
    this->_id = id;
    this->_value = value;
    this->_name = name;
    this->build();
}

HtmlSwitch::~HtmlSwitch()
{
    delete this->_input;
}

void HtmlSwitch::build()
{
    this->addClass("switch");
    this->setId(this->_id.c_str());
    this->_input = new INPUTHtmlNode("checkbox", this->_name.c_str(), (this->_value ? "checked" : ""));
    this->_input->setId((this->_id + "_input").c_str());
    this->addChild(this->_input);

    LABELHtmlNode* label = new LABELHtmlNode();
    label->setAttribute("for", this->_id.c_str());
    label->setId((this->_id + "_label").c_str());
    this->addChild(label);
}

void HtmlSwitch::setValue(bool value)
{
    this->_value = value;
    this->_input->setAttribute("checked", (this->_value ? "checked" : ""));
}

bool HtmlSwitch::getValue()
{
    return this->_value;
}