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

const char *HtmlSlider::getJs()
{
    return "(()=>{window.lastDownTarget=null,document.addEventListener('mousedown',e=>{window.lastDownTarget=e.target},!1);let e={none:'transparent',background:'#ccc',slider:'#2ecc71'},t='change';document.querySelectorAll('.slider').forEach(a=>{if(void 0===a.getContext){console.error('This object does not canvas'),console.error(a);return}let n=a.getContext('2d'),u=a.width,i=a.height,l=!1;function s(){n.clearRect(0,0,u,i),n.strokeStyle=e.none,n.fillStyle=e.background,n.beginPath(),n.roundRect(1,1,u-2,i-2,10),n.stroke(),n.fill(),n.fillStyle=e.slider,n.beginPath();let t=(a.value-a.min)/(a.max-a.min);n.roundRect(1,1,u*t-2,i-2,10),n.stroke(),n.fill(),requestAnimationFrame(s)}function r(e){let t=a.getBoundingClientRect(),n=(e.touches?e.touches[0]:e).clientX-t.left;n<1&&(n=1),n>u-1&&(n=u-1);let i=(n-1)/(u-2)*(a.max-a.min)+a.min;i<a.min&&(i=a.min),i>a.max&&(i=a.max),a.value=Math.round(i/a.step)*a.step,a.setAttribute('value',a.value),a.lastValue!=a.value&&(a.dispatchEvent(new Event('input')),a.lastValue=a.value)}a.max=Number(a.attributes.max?.value||100),a.min=Number(a.attributes.min?.value||0),a.step=Number(a.attributes.step?.value||1),a.value=Number(a.attributes.value?.value||0),a.lastValue=a.value,a.setAttribute('max',a.max),a.setAttribute('min',a.min),a.setAttribute('step',a.step),a.setAttribute('value',a.value),s(),a.addEventListener('mousemove',e=>{!1!=l&&r(e)}),a.addEventListener('touchmove',e=>{!1!=l&&r(e)}),a.addEventListener('click',r),a.addEventListener('mousedown',e=>{l=!0}),document.addEventListener('touchend',e=>{!0==l&&(l=!1,a.dispatchEvent(new Event(t)))}),a.addEventListener('touchstart',e=>{l=!0}),document.addEventListener('mouseup',e=>{!0==l&&(l=!1,a.dispatchEvent(new Event(t)))}),document.addEventListener('keydown',e=>{if(window.lastDownTarget!=a)return;let n=1;e.shiftKey&&(n=10),37==e.keyCode&&(a.value-=a.step*n,a.value<a.min&&(a.value=a.min),a.dispatchEvent(new Event(t))),39==e.keyCode&&(a.value+=a.step*n,a.value>a.max&&(a.value=a.max),a.dispatchEvent(new Event(t)))});let v=new MutationObserver(e=>{e.forEach(e=>{'attributes'===e.type&&('value'===e.attributeName&&(a.value=Number(a.attributes.value?.value||0)),'max'===e.attributeName&&(a.max=Number(a.attributes.max?.value||100)),'min'===e.attributeName&&(a.min=Number(a.attributes.min?.value||0)),'step'===e.attributeName&&(a.step=Number(a.attributes.step?.value||1)))})});v.observe(a,{attributes:!0})})})();";
}

const char *HtmlSlider::getCss()
{
    return ".slider{--slider-width:300px;--slider-height:50px;width:var(--slider-width);height:var(--slider-height);cursor:e-resize;border-radius:10px}";
}