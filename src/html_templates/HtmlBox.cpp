#include "./HtmlBox.h"

HtmlBox::HtmlBox(String title, bool isSubBox, HtmlNode *content, vector<char *> contentClassesNames, bool isInlineContent) : HtmlNode("div", false)
{
    this->_title = title;
    this->_isSubBox = isSubBox;
    this->_isInlineContent = isInlineContent;
    this->build(content, contentClassesNames);
}

HtmlBox::HtmlBox(String title, bool isSubBox, HtmlNode *content, bool isInlineContent) : HtmlNode("div", false)
{
    this->_title = title;
    this->_isSubBox = isSubBox;
    this->_isInlineContent = isInlineContent;
    this->build(content);
}

void HtmlBox::build(HtmlNode *content, vector<char *> contentClassesNames)
{
    this->addClass("box");
    if (this->_isSubBox)
    {
        this->addClass("sub");
    }
    DIVHtmlNode *sub_div = new DIVHtmlNode();
    this->addChild(sub_div);

    DIVHtmlNode *box_header = new DIVHtmlNode();
    sub_div->addChild(box_header);
    box_header->addClass("box-header");

    HtmlNode *titleNode = new HtmlNode("h2", false);
    this->_titleNode = &titleNode;
    titleNode->setInnerText(this->_title.c_str());
    box_header->addChild(titleNode);

    DIVHtmlNode *contentNode = new DIVHtmlNode();
    this->_contentNode = &contentNode;
    sub_div->addChild(contentNode);
    contentNode->addClass("box-content");
    for (auto className : contentClassesNames)
    {
        contentNode->addClass(className);
    }
    if (this->_isInlineContent)
    {
        contentNode->addClass("inline");
    }
    contentNode->addChild(content);
}

HtmlBox::~HtmlBox()
{
    if (this->_titleNode != nullptr)
        delete this->_titleNode;
    if (this->_contentNode != nullptr)
        delete this->_contentNode;
}

void HtmlBox::setTitle(String title)
{
    this->_title = title;
    this->getTitleNode()->setInnerText(this->_title.c_str());
}

void HtmlBox::setContent(HtmlNode *content)
{
    this->getContentNode()->removeAllChildren();
    this->getContentNode()->addChild(content);
}

HtmlNode *HtmlBox::getTitleNode()
{
    return *this->_titleNode;
}

HtmlNode *HtmlBox::getContentNode()
{
    return *this->_contentNode;
}
