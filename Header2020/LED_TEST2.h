// LED Display for TEST & Debug
// Mar.12.2020 by H.Saeki
//

const int LED_B 20;
const int LED_Y 21;
const int LED_G 22;
const int LED_R 23;

// the setup function runs once when you press reset or power the board
void LED_Init() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
}
