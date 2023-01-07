#ifdef PREVENT_SORCING_FROM_ARDUINO

#include "components/open_mv.hpp"
#include "components/setup_handler.hpp"

component::SetupHandler Handler;
component::OpenMV OpenMV(Handler, 19200);

void setup() {
    Handler.Setup();
    Serial.begin(9600);
}

void loop() {
    OpenMV.WaitData();

    const auto ball_cnt = OpenMV.GetBallCount();
    const auto ball_pos = OpenMV.GetBallPosition();
    const auto yello_goal = OpenMV.GetBlueGoalPosition();
    Serial.print("ball_cnt: ");
    Serial.println(ball_cnt);

    Serial.print("ball_pos: (");
    Serial.print(ball_pos.x);
    Serial.print(", ");
    Serial.print(ball_pos.y);
    Serial.println(")");

    Serial.print("yello_goal: (");
    Serial.print(yello_goal.x);
    Serial.print(", ");
    Serial.print(yello_goal.y);
    Serial.println(")");

    Serial.println("");
}

#endif
