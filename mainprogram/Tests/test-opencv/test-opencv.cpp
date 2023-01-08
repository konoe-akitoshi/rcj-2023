#ifdef PREVENT_SORCING_FROM_ARDUINO

#include "components/open_cv.hpp"
#include "components/setup_handler.hpp"

component::SetupHandler Handler;
component::OpenCV OpenCV(Handler, 19200);

void setup() {
    Handler.Setup();
    Serial.begin(9600);
}

void PrintInt(const char* name, const int i) {
    Serial.print(name);
    Serial.print(": ");
    Serial.println(i);
}

void PrintBool(const char* name, const bool b) {
    Serial.print(name);
    Serial.print(": ");
    Serial.println(b ? "true" : "false");
}

void PrintVector(const char* name, const Vector2 vec) {
    Serial.print(name);
    Serial.print(": (");
    Serial.print(vec.x);
    Serial.print(", ");
    Serial.print(vec.y);
    Serial.println(")");
}

void loop() {
    OpenCV.WaitData();

    const auto ball_ext = OpenCV.GetBallExistence();
    const auto ball_pos = OpenCV.GetBallPosition();
    const auto yello_goal_ext = OpenCV.GetYellowGoalExistence();
    const auto yello_goal_pos = OpenCV.GetYellowGoalPosition();
    const auto yello_goal_wdh = OpenCV.GetYellowGoalWidth();
    const auto blue_goal_ext = OpenCV.GetBlueGoalExistence();
    const auto blue_goal_pos = OpenCV.GetBlueGoalPosition();
    const auto blue_goal_wdh = OpenCV.GetBlueGoalWidth();

    PrintBool("ball_ext", ball_ext);
    PrintVector("ball_pos", ball_pos);

    PrintBool("yello_goal_ext", yello_goal_ext);
    PrintVector("yello_goal_pos", yello_goal_pos);
    PrintInt("yello_goal_wdh", yello_goal_wdh);

    PrintBool("blue_goal_ext", blue_goal_ext);
    PrintVector("blue_goal_pos", blue_goal_pos);
    PrintInt("blue_goal_wdh", blue_goal_wdh);

    Serial.println("");
}

#endif