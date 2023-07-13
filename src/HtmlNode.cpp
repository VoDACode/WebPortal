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

HtmlNode::HtmlNode(const char *tag, bool isSelfClosing)
{
    this->tag = new char[strlen(tag) + 1];
    strcpy(this->tag, tag);
    this->tag[strlen(tag)] = '\0';

    this->innerText = NULL;

    this->isSelfClosing = isSelfClosing;
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
    for (size_t i = 0; i < this->children.size(); i++)
    {
        if(this->children[i] != NULL)
            delete this->children[i];
    }
    this->children.clear();
    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if(this->attributes[i] != NULL)
            delete this->attributes[i];
    }
    this->attributes.clear();
    if (this->tag != NULL)
    {
        delete[] this->tag;
    }
    if (this->innerText != NULL)
    {
        delete[] this->innerText;
    }
}

HtmlNode* HtmlNode::clone()
{
    HtmlNode* node = new HtmlNode(this->tag, this->isSelfClosing);
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

HtmlNode &HtmlNode::setInnerText(const char *innerText)
{
    if (this->innerText != NULL)
    {
        delete[] this->innerText;
    }
    this->innerText = new char[strlen(innerText) + 1];
    strcpy(this->innerText, innerText);
    this->innerText[strlen(innerText)] = '\0';
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
            return *this;
        }
    }
    this->attributes.push_back(new HtmlNodeAttribute(name, value));
    return *this;
}

HtmlNode &HtmlNode::removeAttribute(const char *name)
{
    for (size_t i = 0; i < this->attributes.size(); i++)
    {
        if (strcmp(this->attributes[i]->name, name) == 0)
        {
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
            this->children.erase(this->children.begin() + i);
            break;
        }
    }
    return *this;
}

HtmlNode &HtmlNode::removeAllChildren()
{
    for(size_t i = 0; i < this->children.size(); i++){
        this->children[i]->isAdded = false;
        this->children[i]->parent = NULL;
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