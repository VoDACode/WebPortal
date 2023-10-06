#include "HtmlStack.h"

HtmlStack::HtmlStack(HtmlStackOrientation orientation, bool autoSize) : HtmlNode("div", false)
{
    this->orientation = orientation;
    this->autoSize = autoSize;
    this->build();
}

HtmlStack::~HtmlStack()
{
}

void HtmlStack::build()
{
    this->addClass("stack");
    if (this->orientation == STACK_ORIENTATION_HORIZONTAL)
    {
        this->addClass("stack-horizontal");
    }
    
    if(this->autoSize)
    {
        this->addClass("auto-size");
    }
}

void HtmlStack::setOrientation(HtmlStackOrientation orientation)
{
    this->orientation = orientation;
    this->build();
}

HtmlStackOrientation HtmlStack::getOrientation()
{
    return this->orientation;
}

void HtmlStack::addItems(vector<HtmlNode *> items)
{
    for (HtmlNode *item : items)
    {
        this->addItem(item);
    }
}

void HtmlStack::addItem(HtmlNode *item)
{
    this->addChild(item);
}