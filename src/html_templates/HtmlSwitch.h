#pragma once
#include "../HtmlNode.h"

class HtmlSwitch : public HtmlNode
{
    private:
        String _id;
        bool _value;
        String _name;
        void build();
        INPUTHtmlNode* _input;
    public:
        HtmlSwitch(const char *id, const char *name, bool value);
        ~HtmlSwitch();
        void setValue(bool value);
        bool getValue();
};