#include <Arduino.h>
#include <WebPortal.h>
#include <HtmlTemplates.h>

// Wifi config
#define SSID "WiFi SSID"
#define PASSWORD "WiFi Password"

void connectToWifi()
{
  Serial.println("Connecting to WiFi");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

WebPortal webPortal;

HtmlNode *buildPage();

void setup()
{
  Serial.begin(115200);
  webPortal.setPageTitle("Base example");
  webPortal.setTitle("WebPortal - Base example");

  webPortal.setPage(buildPage());
  webPortal.begin();
  connectToWifi();
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

    HtmlStack *buttonExample = new HtmlStack(STACK_ORIENTATION_HORIZONTAL, true);
    DIVHtmlNode *counterBox = new DIVHtmlNode();
    HtmlStack *buttonBox = new HtmlStack(STACK_ORIENTATION_VERTICAL, true);

    LABELHtmlNode *label = new LABELHtmlNode("Click:");
    LABELHtmlNode *counter = new LABELHtmlNode("0");
    BUTTONHtmlNode *clickMe = new BUTTONHtmlNode("button", "Click me");
    BUTTONHtmlNode *setToZero = new BUTTONHtmlNode("button", "Set to zero");
    counterBox->addChild(label);
    counterBox->addChild(counter);
    buttonBox->addChild(clickMe);
    buttonBox->addChild(setToZero);
    buttonExample->addChild(counterBox);
    buttonExample->addChild(buttonBox);
    main->addChild(buttonExample);
    // Bind the click event
    clickMe->onClick(clickHandler, new vector<void *>{counter});
    setToZero->onClick([](const char *eventName, void *node, vector<void *> *context, void *data){
        LABELHtmlNode* counter = (LABELHtmlNode*)(*context)[0];
        counter->setInnerText("0"); 
    }, new vector<void *>{counter});

    DIVHtmlNode *switchStateBox = new DIVHtmlNode();
    LABELHtmlNode *switchLabel = new LABELHtmlNode("Switch:");
    LABELHtmlNode *switchState = new LABELHtmlNode("OFF");
    HtmlSwitch *switchNode = new HtmlSwitch("test_switch", false);
    switchStateBox->addChild(switchLabel);
    switchStateBox->addChild(switchState);
    main->addChild(switchStateBox);
    main->addChild(switchNode);

    switchNode->onChange([](const char *eventName, void *node, vector<void *> *context, void *data) {
        LABELHtmlNode* switchState = (LABELHtmlNode*)(*context)[0];
        HtmlSwitch* switchNode = (HtmlSwitch*)(*context)[1];
        switchState->setInnerText(switchNode->getValue() == true ? "ON" : "OFF"); 
    }, new vector<void *>{switchState, switchNode});

    // Slider box

    DIVHtmlNode *sliderBox = new DIVHtmlNode();

    LABELHtmlNode *sliderLabel = new LABELHtmlNode("Slider:");
    LABELHtmlNode *sliderValue = new LABELHtmlNode("0");
    HtmlSlider *slider = new HtmlSlider("test_slider", 0, -500, 500, 5);
    sliderBox->addChild(sliderLabel);
    sliderBox->addChild(sliderValue);
    main->addChild(sliderBox);
    main->addChild(slider);

    slider->onInput([](const char *eventName, void *node, vector<void *> *context, void *data) {
        LABELHtmlNode* sliderValue = (LABELHtmlNode*)(*context)[0];
        HtmlSlider* slider = (HtmlSlider*)(*context)[1];
        // Serial.println(slider->getValue());
        sliderValue->setInnerText(String(slider->getValue())); 
    }, new vector<void *>{sliderValue, slider});


    // test table

    HtmlBox *tableBox = new HtmlBox("Table", false);
    TABLEHtmlNode *table = new TABLEHtmlNode();

    table->addHeader({      
        new LABELHtmlNode("ID"),
        new LABELHtmlNode("Name"),
        new LABELHtmlNode("Age"),
        new LABELHtmlNode("Actions")
    });

    table->addRow({
        new LABELHtmlNode("1"),
        new LABELHtmlNode("John"),
        new LABELHtmlNode("20"),
        new BUTTONHtmlNode("button", "Delete")
    });

    table->addRow({
        new LABELHtmlNode("2"),
        new LABELHtmlNode("Mary"),
        new LABELHtmlNode("25"),
        new BUTTONHtmlNode("button", "Delete")
    });

    tableBox->addItem(table);
    main->addChild(tableBox);

    // Important: build the main node
    main->buildElement();
    return main;
}