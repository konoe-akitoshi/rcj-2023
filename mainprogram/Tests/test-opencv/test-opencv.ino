#include "components/open_cv.hpp"
#include "components/setup_handler.hpp"
#include "utils/printer.hpp"

component::SetupHandler Handler;
component::OpenCV OpenCV(Handler, 19200);

void setup() {
    Handler.Setup();
    Serial.begin(9600);
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

    util::PrintBool("ball_ext", ball_ext);
    util::PrintVector("ball_pos", ball_pos);

    util::PrintBool("yello_goal_ext", yello_goal_ext);
    util::PrintVector("yello_goal_pos", yello_goal_pos);
    util::PrintInt("yello_goal_wdh", yello_goal_wdh);

    util::PrintBool("blue_goal_ext", blue_goal_ext);
    util::PrintVector("blue_goal_pos", blue_goal_pos);
    util::PrintInt("blue_goal_wdh", blue_goal_wdh);

    Serial.println("");
}
