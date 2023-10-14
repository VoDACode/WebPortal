#include "HtmlSlider.h"

HtmlSlider::HtmlSlider(int value, int min, int max, int step) : CANVASHtmlNode()
{
    this->_value = value;
    this->_min = min;
    this->_max = max;
    this->_step = step;
    this->build();
}

HtmlSlider::HtmlSlider(const char *id, int value, int min, int max, int step) : CANVASHtmlNode(id)
{
    this->setId(id);
    this->_value = value;
    this->_min = min;
    this->_max = max;
    this->_step = step;
    this->build();
}

HtmlSlider::~HtmlSlider()
{
}

void HtmlSlider::build()
{
    this->hasCssCode = true;
    this->hasJsCode = true;
    char value[10];
    itoa(this->_value, value, 10);
    this->setAttribute("value", value);

    itoa(this->_min, value, 10);
    this->setAttribute("min", value);

    itoa(this->_max, value, 10);
    this->setAttribute("max", value);

    itoa(this->_step, value, 10);
    this->setAttribute("step", value);

    this->setAttribute("class", "slider");
    this->on("$beforeEmitEventCallback", [](const char *eventName, void *node, vector<void *> *context, void *data) {
        if(strcmp(eventName, "change") == 0 || strcmp(eventName, "input") == 0){
            int value = atoi((char*)data);
            HtmlSlider *slider = (HtmlSlider *)node;
            slider->setValue(value);
        }
    });
}

void HtmlSlider::setValue(int value)
{
    this->_value = value;
    char val[10];
    itoa(this->_value, val, 10);
    this->setAttribute("value", val);
}

int HtmlSlider::getValue()
{
    return this->_value;
}

void HtmlSlider::setMin(int min)
{
    this->_min = min;
    char val[10];
    itoa(this->_min, val, 10);
    this->setAttribute("min", val);
}

int HtmlSlider::getMin()
{
    return this->_min;
}

void HtmlSlider::setMax(int max)
{
    this->_max = max;
    char val[10];
    itoa(this->_max, val, 10);
    this->setAttribute("max", val);
}

int HtmlSlider::getMax()
{
    return this->_max;
}

void HtmlSlider::setStep(int step)
{
    this->_step = step;
    char val[10];
    itoa(this->_step, val, 10);
    this->setAttribute("step", val);
}

int HtmlSlider::getStep()
{
    return this->_step;
}

void HtmlSlider::onChange(event_callback_t *callback, vector<void*>* context)
{
    this->on("change", callback, context);
}

void HtmlSlider::onInput(event_callback_t *callback, vector<void*>* context)
{
    this->on("input", callback, context);
}

const char *HtmlSlider::componentName()
{
    return "slider";
}