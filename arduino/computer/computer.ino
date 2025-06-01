
#include "core/SDCardManager.h"  
#include "core/PINS.h"
#include "core/StateMachine.h"
#include "SPI.h"

SDCardManager sdManager;

void setup() {
  Serial.begin(921600);

  SPI.begin(PIN_SPI_CLK, PIN_SPI_MISO, PIN_SPI_MOSI);

  if (!sdManager.begin()) {
    Serial.println("SD init failed.");
    return;
  }

  Serial.println("SD Card initialized.");

  // Write a file
  sdManager.writeFile("/test.txt", "Hello from Arduino!\n");

  // Read the file back
  Serial.println("Reading file:");
  sdManager.readFile("/test.txt");

  sdManager.printCardInfo();

  File file = sdManager.getSD().open("/direct.txt", FILE_WRITE);


}

void loop() {

}
