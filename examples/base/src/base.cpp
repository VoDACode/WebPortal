#include <Arduino.h>
#include <WebPortal.h>
#include <HtmlTemplates.h>

WebPortal webPortal;

HtmlNode *buildPage();

void setup()
{
  webPortal.setPageTitle("Base example");
  webPortal.setTitle("WebPortal - Base example");

  webPortal.setPage(buildPage());
  webPortal.begin();
}

void loop()
{
  webPortal.handle();
}

void clickHandler(const char* eventName, void* node, vector<void*>* context, void* data){
  LABELHtmlNode* counter = (LABELHtmlNode*)(*context)[0];
  int click = atoi(counter->getInnerText());
  counter->setInnerText(String(++click));
  Serial.println("Click");
}

HtmlNode *buildPage()
{
  // Create the main node
  MAINHtmlNode *main = new MAINHtmlNode();

  // Create the box for the buttons
  HtmlBox *buttonBox = new HtmlBox("Buttons", false, false);

  LABELHtmlNode *label = new LABELHtmlNode("Click:");
  LABELHtmlNode *counter = new LABELHtmlNode("0");
  BUTTONHtmlNode *clickMe = new BUTTONHtmlNode("button", "Click me");
  BUTTONHtmlNode *setToZero = new BUTTONHtmlNode("button", "Set to zero");
  buttonBox->addItem(label);
  buttonBox->addItem(counter);
  buttonBox->addItem(clickMe);
  buttonBox->addItem(setToZero);
  // Bind the click event
  clickMe->onClick(clickHandler, new vector<void *>{counter});
  setToZero->onClick([](const char *eventName, void *node, vector<void *> *context, void *data){
    LABELHtmlNode* counter = (LABELHtmlNode*)(*context)[0];
    counter->setInnerText("0");
  },  new vector<void *>{counter});

  HtmlBox *switchBox = new HtmlBox("Switch", false, false);
  LABELHtmlNode *switchLabel = new LABELHtmlNode("Switch:");
  LABELHtmlNode *switchState = new LABELHtmlNode("OFF");
  HtmlSwitch *switchNode = new HtmlSwitch("test_switch", false);
  switchBox->addItem(switchLabel);
  switchBox->addItem(switchState);
  switchBox->addItem(switchNode);

  switchNode->onChange([](const char *eventName, void *node, vector<void *> *context, void *data){
    LABELHtmlNode* switchState = (LABELHtmlNode*)(*context)[0];
    HtmlSwitch* switchNode = (HtmlSwitch*)(*context)[1];
    switchState->setInnerText(switchNode->getValue() == true ? "ON" : "OFF");
  },new vector<void *>{switchState, switchNode});

  main->addChild(buttonBox);
  main->addChild(switchBox);
  // Important: build the main node
  main->buildElement();
  return main;
}