#pragma once

// SPI Pins
#define PIN_SPI_CLK  6 // Serial Clock
#define PIN_SPI_MOSI 7 // Master Out, Slave In: Data sent from the master (ESP32) to the slave (display) and SD Card)
#define PIN_SPI_MISO 2 // Master In, Slave Out: Data sent from the slave (SD Card) to the master (ESP32)

#define PIN_SHARP_SS    21 // Sharp Slave Select / Chip Select
#define PIN_SD_SS  20 // SD Card Slave Select / Chip Select