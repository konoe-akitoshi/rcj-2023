// LED Display for TEST & Debug
// Mar.12.2020 by H.Saeki
//

const int LED_BP = 20;
const int LED_YP = 21;
const int LED_GP = 22;
const int LED_RP = 23;

// the setup function runs once when you press reset or power the board
void LED_Init() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BP, OUTPUT);
  pinMode(LED_GP, OUTPUT);
  pinMode(LED_YP, OUTPUT);
  pinMode(LED_RP, OUTPUT);
}

void LED_R(int level){
  if( level == HIGH)
      digitalWrite(LED_RP, HIGH); 
  else
    digitalWrite(LED_RP, LOW); 
  return;
}

void LED_G(int level){
  if( level == HIGH)
      digitalWrite(LED_GP, HIGH); 
  else
    digitalWrite(LED_GP, LOW); 
  return;
}

void LED_Y(int level){
  if( level == HIGH)
      digitalWrite(LED_YP, HIGH); 
  else
    digitalWrite(LED_YP, LOW); 
  return;
}

void LED_B(int level){
  if( level == HIGH)
      digitalWrite(LED_BP, HIGH); 
  else
    digitalWrite(LED_BP, LOW); 
  return;
}
