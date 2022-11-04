/*
  OpenMV serial test
  for multi color version
  for teensy 3.5
  Vcc=5V
  Serial3(TX3,RX3) to OpenMV

  2020.07.31 by saeki

*/

int blob_count, i;
static int x_data_ball, y_data_ball, w_data_ball, h_data_ball;

static int openMV[39];

void setup() {
  Serial.begin(115200);
  Serial3.begin(19200); // initialize serialport for openMV
  Serial.println("Start");
}

void loop() {

  blob_count = get_ball_coordinate();

  x_data_ball = (openMV[5] & 0b0000000000111111) + ((openMV[6] & 0b0000000000111111) << 6);
  y_data_ball = (openMV[7] & 0b0000000000111111) + ((openMV[8] & 0b0000000000111111) << 6);
  w_data_ball = (openMV[9] & 0b0000000000111111) + ((openMV[10] & 0b0000000000111111) << 6);
  h_data_ball = (openMV[11] & 0b0000000000111111) + ((openMV[12] & 0b0000000000111111) << 6);
  Serial.print(" index=");
  Serial.print(openMV[0]);
  Serial.print(" i=");
  Serial.print(openMV[1]);
  Serial.print(" X=");
  Serial.print(x_data_ball);
  Serial.print(" Y=");
  Serial.print(y_data_ball);
  Serial.print(" W=");
  Serial.print(w_data_ball);
  Serial.print(" H=");
  Serial.print(h_data_ball);
  Serial.println();

  x_data_ball = (openMV[18] & 0b0000000000111111) + ((openMV[19] & 0b0000000000111111) << 6);
  y_data_ball = (openMV[20] & 0b0000000000111111) + ((openMV[21] & 0b0000000000111111) << 6);
  w_data_ball = (openMV[22] & 0b0000000000111111) + ((openMV[23] & 0b0000000000111111) << 6);
  h_data_ball = (openMV[24] & 0b0000000000111111) + ((openMV[25] & 0b0000000000111111) << 6);
  Serial.print(" index=");
  Serial.print(openMV[13]);
  Serial.print(" i=");
  Serial.print(openMV[14]);
  Serial.print(" X=");
  Serial.print(x_data_ball);
  Serial.print(" Y=");
  Serial.print(y_data_ball);
  Serial.print(" W=");
  Serial.print(w_data_ball);
  Serial.print(" H=");
  Serial.print(h_data_ball);
  Serial.println();

  x_data_ball = (openMV[31] & 0b0000000000111111) + ((openMV[32] & 0b0000000000111111) << 6);
  y_data_ball = (openMV[33] & 0b0000000000111111) + ((openMV[34] & 0b0000000000111111) << 6);
  w_data_ball = (openMV[35] & 0b0000000000111111) + ((openMV[36] & 0b0000000000111111) << 6);
  h_data_ball = (openMV[37] & 0b0000000000111111) + ((openMV[38] & 0b0000000000111111) << 6);
  Serial.print(" index=");
  Serial.print(openMV[26]);
  Serial.print(" i=");
  Serial.print(openMV[27]);
  Serial.print(" X=");
  Serial.print(x_data_ball);
  Serial.print(" Y=");
  Serial.print(y_data_ball);
  Serial.print(" W=");
  Serial.print(w_data_ball);
  Serial.print(" H=");
  Serial.print(h_data_ball);
  Serial.println();

  delay(500);
}

int get_ball_coordinate() { // get the coordinate data of orange ball
  int i;
  while (Serial3.available() != 0) { // buffer flush
    Serial3.read();
  }
  while ((openMV[0] = getOpenMV()) != 254); // wait for "254"
  for (i = 1; i < 39; i++) {
    openMV[i] = getOpenMV();
  }
  return openMV[0];
}

int getOpenMV() { // get serial data from openMV
  while (Serial3.available() == 0); // wait for serial data
  return Serial3.read();
}
