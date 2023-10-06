#pragma once
#include "HtmlNode.h"

enum HtmlStackOrientation
{
    STACK_ORIENTATION_HORIZONTAL,
    STACK_ORIENTATION_VERTICAL
};

class HtmlStack : public HtmlNode
{
private:
    HtmlStackOrientation orientation;
    bool autoSize = false;
    void build();
public:
    HtmlStack(HtmlStackOrientation orientation = STACK_ORIENTATION_VERTICAL, bool autoSize = false);
    ~HtmlStack();
    
    void setOrientation(HtmlStackOrientation orientation);
    HtmlStackOrientation getOrientation();

    void addItems(vector<HtmlNode *> items);
    void addItem(HtmlNode *item);
};