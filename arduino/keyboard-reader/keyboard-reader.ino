// TODO: 
// - Measure total readout time
// - Base Noise Test 5ms per read, high adc samples
// - Threshold: only send values above 1000
// - Create struct with letter and array of corresponding channel values
// - Print out directly struct code, copy paste into keyboard
// - Send data over UART serial to other esp32


struct KeyMapping {
  int pinA;
  int pinB;
  const char* symbol;  // points to flash string literals
};



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

  // unsigned long t0 = millis();

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
      delayMicroseconds(500); 

  
      // int analogValB = analogRead(ANALOG_IN_B);
      // Serial.print(analogValB);
      // Serial.print(",");

      int adcValue = analogRead(ANALOG_IN_B);
      if (adcValue > 2000) {
        Serial.print("{");
        Serial.print(pinA);
        Serial.print(", ");
        Serial.print(pinB);
        Serial.print(", \"?\"}");
        Serial.println();
      }
    }
  }
  // Serial.println(",0");
}

CD74HC4067* muxes = nullptr;
int mux1Enable = -1;
int mux2Enable = -1;

void selectKeyboardPin(char muxGroup, int pin){

  // unsigned long t0 = millis();
  
  
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

  // unsigned long t1 = millis();
  // Serial.println(t1 - t0);
  
}

// char lastMuxA = 'X'; // '1' or '2'
// char lastMuxB = 'X'; // '1' or '2'

// void selectKeyboardPin(char muxGroup, int pin) {
//   if (pin < 0 || pin > 25) {
//     Serial.println("ERROR: Invalid pin number");
//     return;
//   }

//   CD74HC4067* muxes = nullptr;
//   int mux1Enable = -1;
//   int mux2Enable = -1;
//   char* lastMux = nullptr;
//   char currentMux;

//   // Set up based on group
//   if (muxGroup == 'A') {
//     muxes = &muxesA;
//     mux1Enable = MUX_A1_ENABLE;
//     mux2Enable = MUX_A2_ENABLE;
//     lastMux = &lastMuxA;
//   } else if (muxGroup == 'B') {
//     muxes = &muxesB;
//     mux1Enable = MUX_B1_ENABLE;
//     mux2Enable = MUX_B2_ENABLE;
//     lastMux = &lastMuxB;
//   } else {
//     Serial.println("ERROR: Invalid mux group");
//     return;
//   }

//   if (pin < 13) {
//     currentMux = '1';
//     if (*lastMux != currentMux) delay(1); // Only delay if switching
//     digitalWrite(mux1Enable, LOW);  // Enable MUX 1
//     digitalWrite(mux2Enable, HIGH); // Disable MUX 2
//     muxes->channel(pin);
//   } else {
//     currentMux = '2';
//     if (*lastMux != currentMux) delay(1); // Only delay if switching
//     digitalWrite(mux1Enable, HIGH); // Disable MUX 1
//     digitalWrite(mux2Enable, LOW);  // Enable MUX 2
//     muxes->channel(pin - 13);
//   }

//   *lastMux = currentMux; // Update last used mux
// }


