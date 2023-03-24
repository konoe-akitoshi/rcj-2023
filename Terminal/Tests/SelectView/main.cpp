#include "module/button.hpp"
#include "module/display.hpp"
#include "module/ui/select_view.hpp"

static const String TestSelectViewItems[] = {"This is Item1", "This is Item2", "Item3"};
ui::SelectView TestSelectView(Display, sizeof(TestSelectViewItems) / sizeof(String), TestSelectViewItems, 5);

void setup() {
    do {
        Serial.begin(9600);
        delay(100);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    ButtonLeft.setup();
    StickUp.setup();
    StickDown.setup();
    Serial.println("DONE: setup buttons");

    Display.setup();
    Serial.println("DONE: setup display");

    TestSelectView.show();
}

void loop() {
    if (StickUp.isPressed()) {
        TestSelectView.prev();
        delay(200);

    }
    if (StickDown.isPressed()) {
        TestSelectView.next();
        delay(200);
    }
    if (ButtonLeft.isPressed()) {
        Serial.println(TestSelectView.get());
        delay(200);
    }
}
