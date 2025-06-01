// TODO: 
// - Measure total readout time
// - Base Noise Test 5ms per read, high adc samples
// - Threshold: only send values above 1000
// - Create struct with letter and array of corresponding channel values
// - Print out directly struct code, copy paste into keyboard
// - Send data over UART serial to other esp32

#include <CD74HC4067.h>
#include <Arduino.h>

#define ANALOG_IN_A 0
#define ANALOG_IN_B 1

#define DIGITAL_OUT_A 2
#define DIGITAL_OUT_B 3

#define MUX_A1_ENABLE 23
#define MUX_A2_ENABLE 22
#define MUX_B1_ENABLE 6
#define MUX_B2_ENABLE 7

#define UART_TX 999

CD74HC4067 muxesA(21,20,19,18);
CD74HC4067 muxesB(9,8,15,14);

int analogReadAvg(int pin, int samples = 2) {
  long total = 0;
  for (int i = 0; i < samples; ++i) {
    total += analogRead(pin);
  }
  return total / samples;
}


void setup() {

  // Initialize Digital Output Pins
  pinMode(DIGITAL_OUT_A, OUTPUT);
  pinMode(DIGITAL_OUT_B, OUTPUT);
  digitalWrite(DIGITAL_OUT_A, HIGH);
  digitalWrite(DIGITAL_OUT_B, LOW);


  // Initialize MUX enable pins
  pinMode(MUX_A1_ENABLE, OUTPUT);
  pinMode(MUX_A2_ENABLE, OUTPUT);
  pinMode(MUX_B1_ENABLE, OUTPUT);
  pinMode(MUX_B2_ENABLE, OUTPUT);
  // Set MUX enable pins to HIGH to disable them by default
  digitalWrite(MUX_A1_ENABLE, HIGH);
  digitalWrite(MUX_A2_ENABLE, HIGH);
  digitalWrite(MUX_B1_ENABLE, HIGH);
  digitalWrite(MUX_B2_ENABLE, HIGH);

  // Initialize Analog Input Pins
  pinMode(ANALOG_IN_A, INPUT);
  pinMode(ANALOG_IN_B, INPUT);

  Serial.begin(921600);
}

void loop() {
  measureKeyboard();
}

void measureKeyboard(){

  unsigned long t0 = millis();

  // String buffer;

  // Measure A -> B
//   digitalWrite(DIGITAL_OUT_A, HIGH);
//   digitalWrite(DIGITAL_OUT_B, LOW); 
//   delayMicroseconds(50); 

  for (int pinA = 0; pinA <= 25; pinA++) {
    selectKeyboardPin('A', pinA);
    for (int pinB = 0; pinB <= 25; pinB++) {
      if (pinB == pinA) continue;
      
      selectKeyboardPin('B', pinB);

      delayMicroseconds(50); 

      // int analogValB = analogReadAvg(ANALOG_IN_B);
      int analogValB = analogRead(ANALOG_IN_B);
      Serial.print(analogValB);
      Serial.print(",");
    }
  }

  // Superflous, can be removed as we already reverse pin orders in first loop

  // Measure B -> A
  // digitalWrite(DIGITAL_OUT_A, LOW);
  // digitalWrite(DIGITAL_OUT_B, HIGH); 
  // delayMicroseconds(50); 

  // for (int pinA = 0; pinA <= 25; pinA++) {
  //   selectKeyboardPin('A', pinA);
  //   for (int pinB = 0; pinB <= 25; pinB++) {
  //     if (pinB == pinA) continue;

  //     selectKeyboardPin('B', pinB);

  //     delayMicroseconds(50); 

      

  //     int analogValB = analogReadAvg(ANALOG_IN_B);
  //     Serial.print(analogValB);
  //     Serial.print(",");
  //   }
  // }

  unsigned long t1 = millis();
  // Serial.println(t1 - t0);

  Serial.println(",0");
}

CD74HC4067* muxes = nullptr;
int mux1Enable = -1;
int mux2Enable = -1;

void selectKeyboardPin(char muxGroup, int pin){
  if(pin < 0 || pin > 25) {
    Serial.println("ERROR: Invalid pin number");
    return;
  }

  mux1Enable = -1;
  mux2Enable = -1;

  if (muxGroup == 'A'){
    muxes = &muxesA;
    mux1Enable = MUX_A1_ENABLE;
    mux2Enable = MUX_A2_ENABLE;

  } else if (muxGroup == 'B') {
    muxes = &muxesB;
    mux1Enable = MUX_B1_ENABLE;
    mux2Enable = MUX_B2_ENABLE;
  }

  if (pin < 13) {
    digitalWrite(mux1Enable, LOW); // Activate mux 1
    digitalWrite(mux2Enable, HIGH); // Deactivate mux 2
    muxes->channel(pin); // Select channel 0-12 on mux 1
  } else {
    digitalWrite(mux1Enable, HIGH); // Activate mux 1
    digitalWrite(mux2Enable, LOW); // Deactivate mux 2
    muxes->channel(pin - 13); // Select channel 0-12 on mux 2 (for pins 13-25)
  }
}

