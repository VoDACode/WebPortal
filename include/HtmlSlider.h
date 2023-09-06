#pragma once
#include "HtmlNode.h"

class HtmlSlider : public CANVASHtmlNode
{
private:
    int _value;
    int _min;
    int _max;
    int _step;
    void build();
public:
    HtmlSlider(int value = 0, int min = 0, int max = 100, int step = 1);
    HtmlSlider(const char *id, int value = 0, int min = 0, int max = 100, int step = 1);
    ~HtmlSlider();
    // seters and geters
    void setValue(int value);
    int getValue();
    void setMin(int min);
    int getMin();
    void setMax(int max);
    int getMax();
    void setStep(int step);
    int getStep();
    
    void onChange(event_callback_t *callback, vector<void*>* context);

    const char *getJs();
    const char *getCss();
};