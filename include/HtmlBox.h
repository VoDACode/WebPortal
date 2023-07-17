#pragma once
#include "HtmlNode.h"

class HtmlBox : public HtmlNode
{
private:
    String _title;
    bool _isSubBox;
    bool _isInlineContent;
    HtmlNode *_titleNode;
    DIVHtmlNode *_contentNode;
    DIVHtmlNode *_footerNode;
    void build(HtmlNode *content, vector<char *> contentClassesNames = vector<char *>());
public:
    HtmlBox(String title, bool isSubBox, HtmlNode *content, vector<char *> contentClassesNames, bool isInlineContent = false);
    HtmlBox(String title, bool isSubBox, HtmlNode *content, bool isInlineContent = false);
    HtmlBox(String title, bool isSubBox, bool isInlineContent = false);
    ~HtmlBox();
    void setTitle(String title);
    void setContent(HtmlNode *content);
    HtmlNode *getTitleNode();
    HtmlNode *getContentNode();

    void addItems(vector<HtmlNode *> items);
    void addItem(HtmlNode *item);

    void setFooter(HtmlNode *footer);
    HtmlNode *getFooterNode();

    void addFooterItems(vector<HtmlNode *> items);
    void addFooterItem(HtmlNode *item);
};