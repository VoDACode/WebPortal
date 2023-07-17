#pragma once
#include "../HtmlNode.h"

class HtmlSwitch : public CANVASHtmlNode
{
private:
    bool _value;
    void build();
public:
    HtmlSwitch(bool value = false);
    HtmlSwitch(const char *id, bool value = false);
    ~HtmlSwitch();
    void setValue(bool value);
    bool getValue();
    void onSwitched(event_callback_t *callback, vector<void*>* context);

    const char *getJs();
    const char *getCss();
};