#include "HtmlNode.h"

int HtmlNode::indexOf(const char *str, char c)
{
    const char *ptr = strchr(str, c);
    return ptr != NULL ? ptr - str : -1;
}

void HtmlNode::removeChar(char *str, int index)
{
    if (index >= 0)
    {
        size_t len = strlen(str);
        if (index < len)
        {
            memmove(str + index, str + index + 1, len - index);
        }
        str[len - 1] = '\0';
    }
}

int HtmlNode::numberSize(unsigned long long number)
{
    int size = 0;
    while (number > 0)
    {
        number /= 10;
        size++;
    }
    return size;
}

HtmlNode::HtmlNode(const char *tag, bool isSelfClosing)
{
    this->tag = new char[strlen(tag) + 1];
    strcpy(this->tag, tag);
    this->tag[strlen(tag)] = '\0';

    this->innerText = NULL;
    this->isSelfClosing = isSelfClosing;

    HtmlNodeContainer::getInstance()->addNode(this);
    this->setAttribute("app-id", String(this->id).c_str());
}

HtmlNode::HtmlNode(const HtmlNode &node)
{
    this->tag = new char[strlen(node.tag) + 1];
    strcpy(this->tag, node.tag);
    this->tag[strlen(node.tag)] = '\0';

    if (node.innerText != NULL)
    {
        this->innerText = new char[strlen(node.innerText) + 1];
        strcpy(this->innerText, node.innerText);
        this->innerText[strlen(node.innerText)] = '\0';
    }
    else
    {
        this->innerText = NULL;
    }

    this->isSelfClosing = node.isSelfClosing;
    for (int i = 0; i < node.children.size(); i++)
    {
        this->children.push_back(node.children[i]);
    }
    for (int i = 0; i < node.attributes.size(); i++)
    {
        this->attributes.push_back(node.attributes[i]);
    }
}

HtmlNode::~HtmlNode()
{
    HtmlNodeContainer::getInstance()->removeNode(this->id);
    for (size_t i = 0; i < this->children.size(); i++)
    {
        if (this->children[i] != NULL)
            delete this->children[i];
    }
    this->children.clear();

    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if (this->attributes[i] != NULL)
            delete this->attributes[i];
    }
    this->attributes.clear();

    for (int i = 0; i < events.size(); i++)
    {
        if (events[i] != NULL)
            delete events[i];
    }

    if (this->tag != NULL)
    {
        delete[] this->tag;
    }
    if (this->innerText != NULL)
    {
        delete[] this->innerText;
    }
}

HtmlNode *HtmlNode::clone()
{
    HtmlNode *node = new HtmlNode(this->tag, this->isSelfClosing);
    node->setInnerText(this->innerText);

    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        node->setAttribute(this->attributes[i]->name, this->attributes[i]->value);
    }

    for (size_t i = 0; i < this->children.size(); i++)
    {
        node->addChild(this->children[i]->clone());
    }

    return node;
}

char *HtmlNode::getTag()
{
    return this->tag;
}

HtmlNode &HtmlNode::setId(const char *id)
{
    return this->setAttribute("id", id);
}

char *HtmlNode::getId()
{
    return this->getAttribute("id");
}

HtmlNode &HtmlNode::addClass(const char *className)
{
    HtmlNodeAttribute *attribute = NULL;
    this->getAttribute("class", attribute);
    if (attribute == NULL)
    {
        this->setAttribute("class", className);
    }
    else
    {
        this->setAttribute("class", (String(attribute->value) + " " + String(className)).c_str());
        delete attribute;
    }
    return *this;
}

HtmlNode &HtmlNode::removeClass(const char *className)
{
    char *classValue = this->getAttribute("class");
    if (classValue == NULL)
    {
        return *this;
    }
    String result = String(classValue);
    result.replace(className, "");
    result.replace("  ", " ");
    this->setAttribute("class", result.c_str());
    return *this;
}

vector<const char *> HtmlNode::getClasses()
{
    vector<const char *> result;
    HtmlNodeAttribute *attribute;
    this->getAttribute("class", attribute);
    if (attribute != NULL)
    {
        char *classes = attribute->value;
        int index = indexOf(classes, ' ');
        while (index != -1)
        {
            removeChar(classes, index);
            index = indexOf(classes, ' ');
        }
        char *token;
        char *rest = classes;
        while ((token = strtok_r(rest, " ", &rest)) != NULL)
        {
            result.push_back(token);
        }
        delete attribute;
    }
    return result;
}

// get/set innerText
char *HtmlNode::getInnerText()
{
    if (this->innerText == NULL)
    {
        return NULL;
    }
    return this->innerText;
}

String HtmlNode::getInnerTextString()
{
    if (this->innerText == NULL)
    {
        return "";
    }
    return String(this->innerText);
}
HtmlNode &HtmlNode::setInnerText(String innerText)
{
    return this->setInnerText(innerText.c_str());
}

HtmlNode &HtmlNode::setInnerText(const char *innerText)
{
    if (this->innerText != NULL)
    {
        delete[] this->innerText;
    }
    this->innerText = new char[strlen(innerText) + 1];
    strcpy(this->innerText, innerText);
    this->innerText[strlen(innerText)] = '\0';
    HtmlNodeContainer::getInstance()->updateNode(this);
    return *this;
}

// get/set attributes

const vector<HtmlNodeAttribute *> HtmlNode::getAttributes()
{
    return this->attributes;
}

HtmlNode &HtmlNode::setAttribute(const char *name, const char *value)
{
    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if (strcmp(this->attributes[i]->name, name) == 0)
        {
            if (this->attributes[i]->value != NULL)
                delete[] this->attributes[i]->value;
            this->attributes[i]->value = new char[strlen(value) + 1];
            strcpy(this->attributes[i]->value, value);
            this->attributes[i]->value[strlen(value)] = '\0';

            HtmlNodeContainer::getInstance()->updateNode(this);
            return *this;
        }
    }
    this->attributes.push_back(new HtmlNodeAttribute(name, value));
    HtmlNodeContainer::getInstance()->updateNode(this);
    return *this;
}

HtmlNode &HtmlNode::removeAttribute(const char *name)
{
    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if (strcmp(this->attributes[i]->name, name) == 0)
        {
            HtmlNodeContainer::getInstance()->updateNode(this);
            if (this->attributes[i] != NULL)
                delete this->attributes[i];
            this->attributes.erase(this->attributes.begin() + i);
            break;
        }
    }
    return *this;
}

HtmlNode &HtmlNode::getAttribute(const char *name, HtmlNodeAttribute *&attribute)
{
    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if (strcmp(this->attributes[i]->name, name) == 0)
        {
            attribute = new HtmlNodeAttribute(this->attributes[i]->name, this->attributes[i]->value);
            return *this;
        }
    }
    attribute = NULL;
    return *this;
}

char *HtmlNode::getAttribute(const char *name)
{
    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if (strcmp(this->attributes[i]->name, name) == 0)
        {
            return this->attributes[i]->value;
        }
    }
    return NULL;
}

// add/remove children

HtmlNode &HtmlNode::addChild(HtmlNode *child)
{
    if (child == NULL || child == this || child->isAdded)
    {
        return *this;
    }
    child->isAdded = true;
    child->parent = this;
    this->children.push_back(child);
    // !important: add node to container, if node is not from build-in node
    // HtmlNodeContainer::getInstance()->addUpdatedNode(this);
    return *this;
}

HtmlNode &HtmlNode::removeChild(function<bool(const HtmlNode)> validator)
{
    for (size_t i = 0; i < this->children.size(); i++)
    {
        if (validator(*this->children[i]))
        {
            this->children[i]->isAdded = false;
            this->children[i]->parent = NULL;
            HtmlNodeContainer::getInstance()->removeNode(this->children[i]->id);
            this->children.erase(this->children.begin() + i);
            break;
        }
    }
    return *this;
}

HtmlNode &HtmlNode::removeAllChildren()
{
    for (size_t i = 0; i < this->children.size(); i++)
    {
        this->children[i]->isAdded = false;
        this->children[i]->parent = NULL;
        HtmlNodeContainer::getInstance()->removeNode(this->children[i]->id);
    }
    this->children.clear();
    return *this;
}

vector<HtmlNode *> HtmlNode::getChildren()
{
    return this->children;
}

// hasAttribute

bool HtmlNode::hasAttribute(const char *name)
{
    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if (strcmp(this->attributes[i]->name, name) == 0)
        {
            return true;
        }
    }
    return false;
}

// get isSelfClosing

bool HtmlNode::getIsSelfClosing()
{
    return this->isSelfClosing;
}

size_t HtmlNode::length()
{
    // < + tag
    size_t length = 1 + strlen(this->tag);
    // attributes length
    if (this->attributes.size() > 0)
    {
        for (HtmlNodeAttribute *attribute : this->attributes)
        {
            // (attribute name + = + " + attribute value + ") + space
            length += attribute->length() + 1;
        }
    }
    if (this->isSelfClosing)
    {
        length += 2;
    }
    else
    {
        // >
        length++;
        if (this->innerText != NULL)
        {
            // innerText
            length += strlen(this->innerText);
        }
    }

    // children length
    for (HtmlNode *child : this->children)
    {
        length += child->length();
    }

    // </ + tag + >
    length += 3 + strlen(this->tag);

    return length;
}

size_t HtmlNode::jsLength()
{
    /*
        Example JS result. [INSERT_TO_JS_CODE]:
        window.app.addEvent([this->id],[events[i]->eventName]);
    */
    size_t length = this->getJs() == NULL ? 0 : strlen(this->getJs());
    for (int i = 0; i < events.size(); i++)
    {
        if (events[i]->eventName[0] == '$')
        {
            continue;
        }
        length += strlen(events[i]->eventName) + strlen("window.app.addEvent(,'');") + this->numberSize(this->id);
    }
    // sub element js length
    for (HtmlNode *child : this->children)
    {
        length += child->jsLength();
    }
    return length;
}
// toString

void HtmlNode::toString(char *buffer, int &offset)
{
    strcpy(buffer + offset, "<");
    offset++;
    strcpy(buffer + offset, this->tag);
    offset += strlen(this->tag);
    bool hasAttributes = false;
    if (this->attributes.size() > 0)
    {
        strcpy(buffer + offset, " ");
        offset++;
    }
    for (HtmlNodeAttribute *attribute : this->attributes)
    {
        if (!hasAttributes)
        {
            hasAttributes = true;
        }
        else
        {
            strcpy(buffer + offset, " ");
            offset++;
        }
        attribute->toString(buffer, offset);
    }
    if (this->isSelfClosing)
    {
        strcpy(buffer + offset, "/>");
        offset += 2;
        return;
    }
    else
    {
        strcpy(buffer + offset, ">");
        offset++;
    }

    if (this->innerText != NULL)
    {
        strcpy(buffer + offset, this->innerText);
        offset += strlen(this->innerText);
    }
    for (HtmlNode *child : this->children)
    {
        child->toString(buffer, offset);
    }
    strcpy(buffer + offset, "</");
    offset += 2;
    strcpy(buffer + offset, this->tag);
    offset += strlen(this->tag);
    strcpy(buffer + offset, ">");
    offset++;
}

char *HtmlNode::toString()
{
    int length = this->length();
    char *result = new char[length + 1];
    int offset = 0;
    this->toString(result, offset);
    result[length] = '\0';
    return result;
}

void HtmlNode::generateJsCode(char *buffer, int &offset)
{
    for (int i = 0; i < events.size(); i++)
    {
        if (events[i]->eventName[0] == '$')
        {
            continue;
        }
        strcpy(buffer + offset, "window.app.addEvent(");
        offset += strlen("window.app.addEvent(");
        strcpy(buffer + offset, String(this->id).c_str());
        offset += String(this->id).length();
        strcpy(buffer + offset, ",'");
        offset += strlen(",'");
        strcpy(buffer + offset, events[i]->eventName);
        offset += strlen(events[i]->eventName);
        strcpy(buffer + offset, "');");
        offset += strlen("');");
    }
    if (this->getJs() != NULL)
    {
        strcpy(buffer + offset, this->getJs());
        offset += strlen(this->getJs());
    }
    for (HtmlNode *child : this->children)
    {
        child->generateJsCode(buffer, offset);
    }
}

char *HtmlNode::generateJsCode()
{

    size_t length = this->jsLength();
    char *result = new char[length + 1];
    int offset = 0;
    this->generateJsCode(result, offset);
    result[length] = '\0';
    return result;
}

void HtmlNode::sendCssTo(ResponseContext *context)
{
    if (this->getCss() != NULL)
    {
        context->send(this->getCss());
    }
    for (HtmlNode *child : this->children)
    {
        child->sendCssTo(context);
    }
}

void HtmlNode::buildElement()
{
    this->builded = true;
    for (int i = 0; i < this->children.size(); i++)
    {
        this->children[i]->buildElement();
    }
}

const char *HtmlNode::getJs()
{
    return NULL;
}

const char *HtmlNode::getCss()
{
    return NULL;
}

// events

void HtmlNode::beforeEmit(const char *eventName, vector<void *> *context, void *data)
{
    for (int i = 0; i < events.size(); i++)
    {
        if (strcmp(events[i]->eventName, "$beforeEmitEventCallback") == 0)
        {
            events[i]->callback(eventName, this, events[i]->context, data);
        }
    }
}

void HtmlNode::on(const char *eventName, event_callback_t *callback, vector<void *> *context)
{
    EventData *eventData = new EventData(eventName, callback, context);
    events.push_back(eventData);
}

void HtmlNode::emit(const char *eventName, void *data)
{
    for (int i = 0; i < events.size(); i++)
    {
        if (strcmp(events[i]->eventName, eventName) == 0)
        {
            this->beforeEmit(eventName, events[i]->context, data);
            events[i]->callback(events[i]->eventName, this, events[i]->context, data);
        }
    }
}

void HtmlNode::onClick(event_callback_t *callback, vector<void *> *context)
{
    this->on("click", callback, context);
}

void HtmlNode::onChange(event_callback_t *callback, vector<void *> *context)
{
    this->on("change", callback, context);
}

HtmlNodeContainer *HtmlNodeContainer::instance = NULL;

HtmlNodeContainer::HtmlNodeContainer()
{
    lastId = 0;
}

HtmlNodeContainer::HtmlNodeContainer(const HtmlNodeContainer &HtmlNodeContainer)
{
}

HtmlNodeContainer::~HtmlNodeContainer()
{
}

size_t HtmlNodeContainer::jsonLength()
{
    // strlen("{\"status\":200,\"message\":\"OK\",\"data\":{\"update\":[],\"remove\":[]}}") = 63
    size_t length = 63;
    for (int i = 0; i < updatedNodes.size(); i++)
    {
        if (updatedNodes[i] == NULL)
        {
            continue;
        }
        // strlen("{\"app-id\":,\"attributes\":{},\"innerText\":\"\"}") = 43
        length += 43;
        length += HtmlNode::numberSize(updatedNodes[i]->id);
        for (int j = 0; j < updatedNodes[i]->attributes.size(); j++)
        {
            // "name":"value",
            length += strlen(updatedNodes[i]->attributes[j]->name) + strlen(updatedNodes[i]->attributes[j]->value) + 6;
        }
        // last comma attribute
        length--;
        if (updatedNodes[i]->innerText != NULL)
        {
            length += strlen(updatedNodes[i]->innerText);
        }
    }

    for (int i = 0; i < removedNodes.size(); i++)
    {
        // number + comma
        length += HtmlNode::numberSize(removedNodes[i]) + 1;
    }
    return length;
}

HtmlNodeContainer *HtmlNodeContainer::getInstance()
{
    if (instance == NULL)
    {
        instance = new HtmlNodeContainer();
    }
    return instance;
}

HtmlNode *HtmlNodeContainer::getNode(unsigned long long id)
{
    return nodes[id];
}

HtmlNode *HtmlNodeContainer::addNode(HtmlNode *node)
{
    if (lastId == __LONG_LONG_MAX__)
    {
        lastId = 0;
    }
    node->id = lastId;
    nodes[lastId] = node;
    lastId++;
    return node;
}

void HtmlNodeContainer::removeNode(unsigned long long id)
{
    if (find(removedNodes.begin(), removedNodes.end(), id) == removedNodes.end())
    {
        removedNodes.push_back(id);
        nodes.erase(id);
    }
}

void HtmlNodeContainer::removeAllNodes()
{
    for (auto it = nodes.begin(); it != nodes.end(); ++it)
    {
        removedNodes.push_back(it->second->id);
    }
    nodes.clear();
}

void HtmlNodeContainer::updateNode(HtmlNode *node)
{
    if (node == NULL)
    {
        return;
    }
    if (node->builded)
    {
        if (updatedNodes.find(node->id) == updatedNodes.end())
        {
            // remove node from updatedNodes
            updatedNodes.erase(node->id);
        }
        updatedNodes[node->id] = node;
    }
}

char *HtmlNodeContainer::handleUpdatedNodes()
{
    if (updatedNodes.size() == 0 && removedNodes.size() == 0)
    {
        return NULL;
    }
    // Serial.printf("Updated nodes: %d\n", updatedNodes.size());
    // Serial.printf("Removed nodes: %d\n", removedNodes.size());
    // build JSON response like this:
    /*
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
                "remove": [1,2,10],   // list of app-id
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

    size_t length = this->jsonLength();
    char *result = new char[length + 1];
    int offset = 0;

    strcpy(result + offset, "{\"status\":200,\"message\":\"OK\",\"data\":{\"update\":[");
    offset += 47;

    for (size_t i = 0; i < updatedNodes.size(); i++)
    {
        // Serial.printf("Updated node is %s\n", updatedNodes[i] == NULL ? "NULL" : updatedNodes[i]->tag);
        if (updatedNodes[i] == NULL)
        {
            continue;
        }
        strcpy(result + offset, "{\"app-id\":");
        offset += 10;
        strcpy(result + offset, String(updatedNodes[i]->id).c_str());
        offset += String(updatedNodes[i]->id).length();
        strcpy(result + offset, ",\"attributes\":{");
        offset += 15;
        for (size_t j = 0; j < updatedNodes[i]->attributes.size(); j++)
        {
            strcpy(result + offset, "\"");
            offset++;
            strcpy(result + offset, updatedNodes[i]->attributes[j]->name);
            offset += strlen(updatedNodes[i]->attributes[j]->name);
            strcpy(result + offset, "\":\"");
            offset += 3;
            strcpy(result + offset, updatedNodes[i]->attributes[j]->value);
            offset += strlen(updatedNodes[i]->attributes[j]->value);
            strcpy(result + offset, "\"");
            offset++;
            if (j < updatedNodes[i]->attributes.size() - 1)
            {
                strcpy(result + offset, ",");
                offset++;
            }
        }
        strcpy(result + offset, "}");
        offset++;
        strcpy(result + offset, ",\"innerText\":\"");
        offset += 14;
        if (updatedNodes[i]->innerText != NULL)
        {
            strcpy(result + offset, updatedNodes[i]->innerText);
            offset += strlen(updatedNodes[i]->innerText);
        }
        strcpy(result + offset, "\"}");
        offset += 2;
        if (i < updatedNodes.size() - 1)
        {
            strcpy(result + offset, ",");
            offset++;
        }
    }
    strcpy(result + offset, "],\"remove\":[");
    offset += 12;
    for (size_t i = 0; i < removedNodes.size(); i++)
    {
        strcpy(result + offset, String(removedNodes[i]).c_str());
        offset += String(removedNodes[i]).length();
        if (i < removedNodes.size() - 1)
        {
            strcpy(result + offset, ",");
            offset++;
        }
    }
    strcpy(result + offset, "]}}");
    offset += 3;

    result[length] = '\0';

    updatedNodes.clear();
    removedNodes.clear();

    return result;
}