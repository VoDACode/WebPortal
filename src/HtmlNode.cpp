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
    HtmlNodeAttribute attribute;
    this->getAttribute("id", attribute);
    return attribute.value;
}

HtmlNode &HtmlNode::addClass(const char *className)
{
    HtmlNodeAttribute attribute;
    this->getAttribute("class", attribute);
    if (attribute.name == NULL)
    {
        this->setAttribute("class", className);
    }
    else
    {
        // concat use char method
        char *arr = new char[strlen(attribute.value) + strlen(className) + 1];
        strcpy(arr, attribute.value);
        strcat(arr, " ");
        strcat(arr, className);
        this->setAttribute("class", arr);
        delete[] arr;
    }
    return *this;
}

HtmlNode &HtmlNode::removeClass(const char *className)
{
    HtmlNodeAttribute attribute;
    this->getAttribute("class", attribute);
    if (attribute.name[0] != '\0')
    {
        char *classes = attribute.value;
        size_t classNameLength = strlen(className);
        char *match = strstr(classes, className);
        while (match != NULL)
        {
            memmove(match, match + classNameLength, strlen(match + classNameLength) + 1);
            match = strstr(classes, className);
        }
        this->setAttribute("class", classes);
    }
    return *this;
}

vector<char *> HtmlNode::getClasses()
{
    vector<char *> result;
    HtmlNodeAttribute attribute;
    this->getAttribute("class", attribute);
    if (attribute.name[0] != '\0')
    {
        char *classes = attribute.value;
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
            this->attributes.erase(this->attributes.begin() + i);
            break;
        }
    }
    return *this;
}

char *HtmlNode::getAttributeValue(const char *name)
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

HtmlNode &HtmlNode::getAttribute(const char *name, HtmlNodeAttribute &attribute)
{
    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if (strcmp(this->attributes[i]->name, name) == 0)
        {
            attribute = *this->attributes[i];
            return *this;
        }
    }
    attribute = HtmlNodeAttribute();
    return *this;
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
    //HtmlNodeContainer::getInstance()->addUpdatedNode(this);
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
    HtmlNodeContainer::getInstance()->updateNode(this);
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
    size_t length = 0;
    for (int i = 0; i < events.size(); i++)
    {
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

// events

void HtmlNode::on(const char *eventName, event_callback_t* callback, void *data)
{
    EventData *eventData = new EventData(eventName, callback, data);
    events.push_back(eventData);
}

void HtmlNode::emit(const char *eventName, void *data)
{
    for (int i = 0; i < events.size(); i++)
    {
        if (strcmp(events[i]->eventName, eventName) == 0)
        {
            events[i]->callback(events[i]->eventName, this, events[i]->data, data);
        }
    }
}

void HtmlNode::onClick(event_callback_t* callback, void *data)
{
    this->on("click", callback, data);
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
    nodes.erase(id);
    if (addedNodes.find(id) != addedNodes.end())
        addedNodes.erase(id);
    removedNodes.push_back(id);
}

void HtmlNodeContainer::removeAllNodes()
{
    for (auto it = nodes.begin(); it != nodes.end(); ++it)
    {
        removedNodes.push_back(it->second->id);
        delete it->second;
    }
    nodes.clear();
}

void HtmlNodeContainer::updateNode(HtmlNode *node)
{
    updatedNodes[node->id] = node;
}

void HtmlNodeContainer::addUpdatedNode(HtmlNode *node)
{
    addedNodes[node->id] = node;
}

char *HtmlNodeContainer::handleUpdatedNodes()
{
    if (updatedNodes.size() == 0 && removedNodes.size() == 0 && addedNodes.size() == 0)
    {
        return NULL;
    }
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

    StaticJsonDocument<MAX_JSON_RESPONSE_SIZE> doc;
    doc["status"] = 200;
    doc["message"] = "OK";
    JsonObject data = doc.createNestedObject("data");
    JsonArray update = data.createNestedArray("update");
    JsonArray remove = data.createNestedArray("remove");
    JsonArray add = data.createNestedArray("add");

    for (auto it = updatedNodes.begin(); it != updatedNodes.end(); ++it)
    {
        JsonObject obj = update.createNestedObject();
        obj["app-id"] = it->second->id;
        JsonObject attributes = obj.createNestedObject("attributes");
        for (int i = 0; i < it->second->attributes.size(); i++)
        {
            attributes[it->second->attributes[i]->name] = it->second->attributes[i]->value;
        }
        if (it->second->innerText != NULL)
        {
            obj["innerText"] = it->second->innerText;
        }
    }

    for (int i = 0; i < removedNodes.size(); i++)
    {
        remove.add(removedNodes[i]);
    }

    for(int i = 0; i < addedNodes.size(); i++){
        JsonObject obj = add.createNestedObject();
        if(addedNodes[i]->parent != NULL)
            obj["parent"] = addedNodes[i]->parent->id;
        else
            obj["parent"] = -1;
        obj["tag"] = addedNodes[i]->tag;
        obj["app-id"] = addedNodes[i]->id;
        JsonObject attributes = obj.createNestedObject("attributes");
        for (int i = 0; i < addedNodes[i]->attributes.size(); i++)
        {
            attributes[addedNodes[i]->attributes[i]->name] = addedNodes[i]->attributes[i]->value;
        }
        if (addedNodes[i]->innerText != NULL)
        {
            obj["innerText"] = addedNodes[i]->innerText;
        }
    }

    updatedNodes.clear();
    removedNodes.clear();
    addedNodes.clear();

    size_t length = measureJson(doc);
    char *result = new char[length + 1];
    serializeJson(doc, result, length + 1);
    return result;
}