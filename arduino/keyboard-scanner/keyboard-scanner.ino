struct KeyMapping {
  int pinA;
  int pinB;
  const char* symbol;  // Use strings for normal chars and special keys
};

// Example list with normal keys and special keys as strings
KeyMapping keyboardMap[] = {
    {8, 23, "1"},
    {23, 8, "1"},
    {7, 23, "2"},
    {23, 7, "2"},
    {23, 3, "3"},
    {3, 23, "3"},
    {9, 22, "4"},
    {22, 9, "4"},
    {2, 9, "5"},
    {9, 2, "5"},
    {18, 2, "6"},
    {18, 22, "7"},
    {17, 23, "8"},
    {23, 17, "8"},
    {16, 23, "9"},
    {23, 16, "9"},
    {23, 15, "0"},
    {15, 23, "0"},
    {15, 25, "?"},
    {25, 15, "?"},
    {14, 0, "BACKSPACE"},
    {0, 14, "BACKSPACE"},
    {8, 21, "q"},
    {21, 8, "q"},
    {21, 7, "w"},
    {7, 21, "w"},
    {3, 21, "e"},
    {21, 3, "e"},
    {21, 9, "r"},
    {9, 21, "r"},
    {20, 9, "t"},
    {9, 20, "t"},
    {18, 20, "z"},
    {18, 21, "u"},
    {21, 17, "i"},
    {17, 21, "i"},
    {21, 16, "o"},
    {16, 21, "o"},
    {15, 21, "p"},
    {21, 15, "p"},
    {14, 22, "è"},
    {22, 14, "è"},
    {22, 8, "a"},
    {8, 22, "a"},
    {7, 22, "s"},
    {22, 7, "s"},
    {22, 3, "d"},
    {3, 22, "d"},
    {9, 23, "f"},
    {23, 9, "f"},
    {9, 0, "g"},
    {0, 9, "g"},
    {18, 0, "h"},
    {18, 23, "j"},
    {17, 22, "k"},
    {22, 17, "k"},
    {16, 22, "l"},
    {22, 16, "l"},
    {14, 22, "é"},
    {22, 14, "é"},
    {17, 2, "à"},
    {2, 17, "à"},
    {15, 20, "$"},
    {20, 15, "$"},
    {14, 20, "ENTER"},
    {20, 14, "ENTER"},
    {7, 0, "SHIFT"},
    {0, 7, "SHIFT"},
    {20, 8, "y"},
    {8, 20, "y"},
    {7, 20, "x"},
    {20, 7, "x"},
    {20, 3, "c"},
    {3, 20, "c"},
    {9, 1, "v"},
    {1, 9, "v"},
    {9, 25, "b"},
    {25, 9, "b"},
    {18, 25, "n"},
    {18, 25, "n"},
    {18, 1, "m"},
    {18, 1, "m"},
    {17, 20, ","},
    {20, 17, ","},
    {20, 16, "."},
    {16, 20, "."},
    {15, 2, "-"},
    {2, 15, "-"},
    {16, 2, "SHIFT"},
    {2, 16, "SHIFT"},
    {24, 5, "CTRL"},
    {5, 24, "CTRL"},
    {4, 24, "ALT"},
    {24, 4, "ALT"},
    {19, 1, " "},
    {1, 19, " "},
    {24, 10, "ALT_GR"},
    {10, 24, "ALT_GR"},
    {24, 11, "CTRL"},
    {11, 24, "CTRL"},
    {13, 0, "LEFT_ARROW"},
    {0, 13, "LEFT_ARROW"},
    {25, 13, "RIGTH_ARROW"},
    {13, 25, "RIGTH_ARROW"},
    {14, 2, "UP_ARROW"},
    {2, 14, "UP_ARROW"},
    {0, 19, "DOWN_ARROW"},
    {19, 0, "DOWN_ARROW"},
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
  measureKeyboardFromMap();
}

void measureKeyboardFromMap() {
  int lastPinA = -1;
  int lastPinB = -1;

  const int mapSize = sizeof(keyboardMap) / sizeof(KeyMapping);

  for (int i = 0; i < mapSize; ++i) {
    KeyMapping& k = keyboardMap[i];

    // Only change pin A if needed
    if (k.pinA != lastPinA) {
      selectKeyboardPin('A', k.pinA);
      lastPinA = k.pinA;
    }

    // Only change pin B if needed
    if (k.pinB != lastPinB) {
      selectKeyboardPin('B', k.pinB);
      lastPinB = k.pinB;
    }

    delayMicroseconds(100);  // Allow signal to settle

    int adcValue = analogRead(ANALOG_IN_B);
    if (adcValue > 2200) {
      Serial.println(k.symbol);  // Print just the symbol
    }
  }
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