#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <Arduino.h>
#include <map>
#include <HttpClient.h>

using namespace std;

class HtmlNode;

typedef void (event_callback_t)(const char *eventName, void* node, vector<void*>* context, void *data);

struct EventData
{
    char *eventName;
    vector<void*>* context;
    event_callback_t *callback;

    EventData(const char *eventName, event_callback_t *callback, vector<void*>* context = NULL)
    {
        this->eventName = new char[strlen(eventName) + 1];
        strcpy(this->eventName, eventName);
        this->eventName[strlen(eventName)] = '\0';
        this->context = context;
        this->callback = callback;
    }

    ~EventData()
    {
        delete[] eventName;
    }
};

struct HtmlNodeAttribute
{
    char *name = NULL;
    char *value = NULL;

    HtmlNodeAttribute(const char *name, const char *value)
    {
        this->name = new char[strlen(name) + 1];
        this->value = new char[strlen(value) + 1];
        strcpy(this->name, name);
        this->name[strlen(name)] = '\0';
        strcpy(this->value, value);
        this->value[strlen(value)] = '\0';
    }

    HtmlNodeAttribute()
    {
    }

    ~HtmlNodeAttribute()
    {
        if (this->name != NULL)
        {
            delete[] this->name;
        }
        if (this->value != NULL)
        {
            delete[] this->value;
        }
    }
    void toString(char *buffer, int &offset)
    {
        if (this->name != NULL)
        {
            strcpy(buffer + offset, this->name);
            offset += strlen(this->name);
        }
        strcpy(buffer + offset, "=\"");
        offset += 2;
        if (this->value != NULL)
        {
            strcpy(buffer + offset, this->value);
            offset += strlen(this->value);
        }
        strcpy(buffer + offset, "\"");
        offset++;
    }
    
    char *toString()
    {
        char *result = new char[this->length() + 1];
        int offset = 0;
        this->toString(result, offset);
        return result;
    }

    size_t length()
    {
        // attribute name + = + " + attribute value + "
        return (this->name != NULL ? strlen(this->name) : 0) + 2 + (this->value != NULL ? strlen(this->value) : 0) + 1;
    }
};

class HtmlNode
{
    friend class HtmlNodeContainer;
    friend class WebPortal;
private:
    vector<EventData *> events;
    int indexOf(const char *str, char c);
    void removeChar(char *str, int index);
    void toString(char *buffer, int &offset);
    void generateJsCode(char *buffer, int &offset);
    // this parameter true when this node is added to another node
    bool isAdded = false;

    unsigned long long id = 0;

    char *tag;
    char *innerText;
    bool isSelfClosing;
    HtmlNode *parent;
    vector<HtmlNode *> children;
    vector<HtmlNodeAttribute *> attributes;

    bool builded = false;

    void beforeEmit(const char *eventName, vector<void*>* context, void *data);

    void sendCssTo(HttpClient *client);

protected:
    static int numberSize(unsigned long long number);
    size_t jsLength();
    event_callback_t* beforeEmitEventCallback = NULL;
    // TO DO
    virtual const char* getJs();
    virtual const char* getCss();
public:
    HtmlNode(const char *tag, bool isSelfClosing);
    HtmlNode(const HtmlNode &HtmlNode);
    ~HtmlNode();

    HtmlNode *clone();

    char *getTag();

    HtmlNode &setId(const char *id);
    char *getId();

    HtmlNode &addClass(const char *className);
    HtmlNode &removeClass(const char *className);
    vector<char *> getClasses();

    // get/set innerText
    char *getInnerText();
    String getInnerTextString();
    HtmlNode &setInnerText(const char *innerText);
    HtmlNode &setInnerText(String innerText);

    // add/remove children
    HtmlNode &addChild(HtmlNode *child);
    HtmlNode &removeChild(function<bool(const HtmlNode)> validator);
    HtmlNode &removeAllChildren();
    vector<HtmlNode *> getChildren();

    // get/set/remove attributes; set - add or replace
    const vector<HtmlNodeAttribute *> getAttributes();
    HtmlNode &setAttribute(const char *name, const char *value);
    HtmlNode &removeAttribute(const char *name);
    char *getAttributeValue(const char *name);
    HtmlNode &getAttribute(const char *name, HtmlNodeAttribute &attribute);

    // hasAttribute
    bool hasAttribute(const char *name);

    // get isSelfClosing
    bool getIsSelfClosing();

    size_t length();

    // toString
    char *toString();

    char *generateJsCode();

    void buildElement();

    // events
    void on(const char *eventName, event_callback_t *callback, vector<void*>* context = NULL);
    void emit(const char *eventName, void *data);
    virtual void onClick(event_callback_t *callback, vector<void*>* context = NULL);
    virtual void onChange(event_callback_t *callback, vector<void*>* context = NULL);
};

class HtmlNodeContainer
{
    // singleton
private:
    std::vector<unsigned long long> removedNodes;
    std::map<unsigned long long, HtmlNode *> updatedNodes;

    std::map<unsigned long long, HtmlNode *> nodes;
    unsigned long long lastId;
    HtmlNodeContainer();
    HtmlNodeContainer(const HtmlNodeContainer &HtmlNodeContainer);
    ~HtmlNodeContainer();
    static HtmlNodeContainer *instance;

    size_t jsonLength();

public:
    static HtmlNodeContainer *getInstance();
    HtmlNode *getNode(unsigned long long id);
    HtmlNode *addNode(HtmlNode *node);
    void removeNode(unsigned long long id);
    void removeAllNodes();

    void updateNode(HtmlNode *node);

    char *handleUpdatedNodes();
};

// #region HTML items

class DOCUMENTHtmlNode : public HtmlNode
{
public:
    DOCUMENTHtmlNode() : HtmlNode("!DOCTYPE html", true) {}
};

class HTMLHtmlNode : public HtmlNode
{
public:
    HTMLHtmlNode() : HtmlNode("html", false) {}
};

class HEADHtmlNode : public HtmlNode
{
public:
    HEADHtmlNode() : HtmlNode("head", false) {}
};

class TITLEHtmlNode : public HtmlNode
{
public:
    TITLEHtmlNode() : HtmlNode("title", false) {}
    TITLEHtmlNode(char *innerText) : HtmlNode("title", false)
    {
        this->setInnerText(innerText);
    }
};

class METAHtmlNode : public HtmlNode
{
public:
    METAHtmlNode() : HtmlNode("meta", true) {}
    METAHtmlNode(char *name, char *content) : HtmlNode("meta", true)
    {
        this->setAttribute("name", name);
        this->setAttribute("content", content);
    }
};

class LINKHtmlNode : public HtmlNode
{
public:
    LINKHtmlNode() : HtmlNode("link", true) {}
    LINKHtmlNode(char *rel, char *href) : HtmlNode("link", true)
    {
        this->setAttribute("rel", rel);
        this->setAttribute("href", href);
    }

    LINKHtmlNode(const char *rel) : HtmlNode("link", true)
    {
        this->setAttribute("rel", rel);
    }
};

class STYLEHtmlNode : public HtmlNode
{
public:
    STYLEHtmlNode() : HtmlNode("style", false) {}
    STYLEHtmlNode(const char *innerText) : HtmlNode("style", false)
    {
        this->setInnerText(innerText);
    }
};

class SCRIPTHtmlNode : public HtmlNode
{
public:
    SCRIPTHtmlNode() : HtmlNode("script", false) {}
    SCRIPTHtmlNode(const char *src) : HtmlNode("script", false)
    {
        this->setAttribute("src", src);
    }
};

class BODYHtmlNode : public HtmlNode
{
public:
    BODYHtmlNode() : HtmlNode("body", false) {}
};

class MAINHtmlNode : public HtmlNode
{
public:
    MAINHtmlNode() : HtmlNode("main", false) {}
};

class DIVHtmlNode : public HtmlNode
{
public:
    DIVHtmlNode() : HtmlNode("div", false) {}
};

class PHtmlNode : public HtmlNode
{
public:
    PHtmlNode() : HtmlNode("p", false) {}
};

class AHtmlNode : public HtmlNode
{
public:
    AHtmlNode() : HtmlNode("a", false) {}
    AHtmlNode(const char *href, const char *innerText) : HtmlNode("a", false)
    {
        this->setAttribute("href", href);
        this->setInnerText(innerText);
    }
    AHtmlNode(const char *href) : HtmlNode("a", false)
    {
        this->setAttribute("href", href);
    }
};

class IMGHtmlNode : public HtmlNode
{
public:
    IMGHtmlNode() : HtmlNode("img", true) {}
    IMGHtmlNode(const char *src) : HtmlNode("img", true)
    {
        this->setAttribute("src", src);
    }
};

class ULHtmlNode : public HtmlNode
{
public:
    ULHtmlNode() : HtmlNode("ul", false) {}
};

class LIHtmlNode : public HtmlNode
{
public:
    LIHtmlNode() : HtmlNode("li", false) {}
};

class TABLEHtmlNode : public HtmlNode
{
public:
    TABLEHtmlNode() : HtmlNode("table", false) {}
};

class THEADHtmlNode : public HtmlNode
{
public:
    THEADHtmlNode() : HtmlNode("thead", false) {}
};

class TBODYHtmlNode : public HtmlNode
{
public:
    TBODYHtmlNode() : HtmlNode("tbody", false) {}
};

class TRHtmlNode : public HtmlNode
{
public:
    TRHtmlNode() : HtmlNode("tr", false) {}
};

class THHtmlNode : public HtmlNode
{
public:
    THHtmlNode() : HtmlNode("th", false) {}
};

class TDHtmlNode : public HtmlNode
{
public:
    TDHtmlNode() : HtmlNode("td", false) {}
};

class FORMHtmlNode : public HtmlNode
{
public:
    FORMHtmlNode() : HtmlNode("form", false) {}
    FORMHtmlNode(const char *action, const char *method) : HtmlNode("form", false)
    {
        this->setAttribute("action", action);
        this->setAttribute("method", method);
    }
};

class INPUTHtmlNode : public HtmlNode
{
public:
    INPUTHtmlNode() : HtmlNode("input", true) {}
    INPUTHtmlNode(const char *type, const char *name, const char *value) : HtmlNode("input", true)
    {
        this->setAttribute("type", type);
        this->setAttribute("name", name);
        this->setAttribute("value", value);
    }
};

class INPUT_NUMBERHtmlNode : public INPUTHtmlNode
{
public:
    INPUT_NUMBERHtmlNode() : INPUTHtmlNode("num", "", "") {}
    INPUT_NUMBERHtmlNode(const char *name, const char *value) : INPUTHtmlNode("num", name, value) {}

    void supportNegative(bool supportNegative)
    {
        this->setAttribute("negative", supportNegative ? "true" : "false");
    }

    bool isSupportNegative()
    {
        return strcmp(this->getAttributeValue("negative"), "true") == 0;
    }

    void supportFloat(bool supportFloat)
    {
        this->setAttribute("float", supportFloat ? "true" : "false");
    }

    bool isSupportFloat()
    {
        return strcmp(this->getAttributeValue("float"), "true") == 0;
    }

    void setMinValue(int minValue)
    {
        char temp[10];
        itoa(minValue, temp, 10);
        this->setAttribute("min", temp);
    }

    int getMinValue()
    {
        return atoi(this->getAttributeValue("min"));
    }

    void setMaxValue(int maxValue)
    {
        char temp[10];
        itoa(maxValue, temp, 10);
        this->setAttribute("max", temp);
    }

    int getMaxValue()
    {
        return atoi(this->getAttributeValue("max"));
    }
};

class CANVASHtmlNode : public HtmlNode
{
public:
    CANVASHtmlNode() : HtmlNode("canvas", false) {}
    CANVASHtmlNode(const char *id) : HtmlNode("canvas", false)
    {
        this->setId(id);
    }
};

class BUTTONHtmlNode : public HtmlNode
{
public:
    BUTTONHtmlNode() : HtmlNode("button", false) {}
    BUTTONHtmlNode(const char *type, const char *innerText) : HtmlNode("button", false)
    {
        this->setAttribute("type", type);
        this->setInnerText(innerText);
    }
};

class TEXTAREAHtmlNode : public HtmlNode
{
public:
    TEXTAREAHtmlNode() : HtmlNode("textarea", false) {}
    TEXTAREAHtmlNode(const char *name) : HtmlNode("textarea", false)
    {
        this->setAttribute("name", name);
    }
};

class OPTIONHtmlNode : public HtmlNode
{
public:
    OPTIONHtmlNode() : HtmlNode("option", false) {}
    OPTIONHtmlNode(const char *value, const char *innerText) : HtmlNode("option", false)
    {
        this->setAttribute("value", value);
        this->setInnerText(innerText);
    }

    void setSelected(bool selected)
    {
        if (selected)
            this->setAttribute("selected", "selected");
        else
            this->removeAttribute("selected");
    }

    bool isSelected()
    {
        return this->hasAttribute("selected");
    }

    void setDisabled(bool disabled)
    {
        if (disabled)
            this->setAttribute("disabled", "disabled");
        else
            this->removeAttribute("disabled");
    }

    bool isDisabled()
    {
        return this->hasAttribute("disabled");
    }

    void setDefault(bool defaultOption)
    {
        if (defaultOption)
            this->setAttribute("default", "default");
        else
            this->removeAttribute("default");
    }

    bool isDefault()
    {
        return this->hasAttribute("default");
    }
};

class SELECTHtmlNode : public HtmlNode
{
protected:
    OPTIONHtmlNode *selectedOption;
    OPTIONHtmlNode *defaultOption;

public:
    SELECTHtmlNode() : HtmlNode("select", false) {}
    SELECTHtmlNode(const char *name) : HtmlNode("select", false)
    {
        this->setAttribute("name", name);
    }

    HtmlNode &addOption(const char *value, const char *innerText, bool selected = false, bool defaultOption = false)
    {
        OPTIONHtmlNode *option = new OPTIONHtmlNode(value, innerText);
        if (selected)
        {
            if (this->selectedOption != NULL)
                this->selectedOption->setSelected(false);
            this->selectedOption = option;
        }
        if (defaultOption)
        {
            if (this->defaultOption != NULL)
                this->defaultOption->setDefault(false);
            this->defaultOption = option;
        }
        option->setSelected(selected);
        this->addChild(option);
        return *this;
    }

    HtmlNode &addOption(OPTIONHtmlNode *option)
    {
        if (option->isSelected())
        {
            if (this->selectedOption != NULL)
                this->selectedOption->setSelected(false);
            this->selectedOption = option;
        }
        if (option->isDefault())
        {
            if (this->defaultOption != NULL)
                this->defaultOption->setDefault(false);
            this->defaultOption = option;
        }
        this->addChild(option);
        return *this;
    }

    HtmlNode &addOption(const char *value)
    {
        OPTIONHtmlNode *option = new OPTIONHtmlNode(value, value);
        this->addChild(option);
        return *this;
    }
};

class LABELHtmlNode : public HtmlNode
{
public:
    LABELHtmlNode() : HtmlNode("label", false) {}
    LABELHtmlNode(const char *innerText) : HtmlNode("label", false)
    {
        this->setInnerText(innerText);
    }
    HtmlNode &setFor(const char *forId)
    {
        this->setAttribute("for", forId);
        return *this;
    }

    HtmlNode &setFor(HtmlNode &forItem)
    {
        this->setAttribute("for", forItem.getId());
        return *this;
    }

    HtmlNode &setFor(HtmlNode *forItem)
    {
        this->setAttribute("for", forItem->getId());
        return *this;
    }

    char *getFor()
    {
        HtmlNodeAttribute attr;
        this->getAttribute("for", attr);
        return attr.value;
    }
};

// #endregion