#include <RadioLib.h>
#include <fstream>
#include <string>
#include "PiHal.h"

// Create a new instance of the HAL class
PiHal* hal = new PiHal(0);

// Create the radio module instance
// Pinout corresponds to your SX1262 setup
// NSS pin: WiringPi 10 (GPIO 8)
// DIO1 pin: WiringPi 2 (GPIO 27)
// NRST pin: WiringPi 21 (GPIO 5)
// BUSY pin: WiringPi 0 (GPIO 17)
SX1262 radio = new Module(hal, 10, 2, 21, 0);

int main() {
  // Initialize the radio module with XTAL configuration
  printf("[SX1262] Initializing ... ");
  //int state = radio.beginFSK(915.0, 4.8, 125.0, 467.0, 10.0, 16.0, 0.0, false);
  int state = radio.begin(915.0, 125.0, 7, 5, 0, 10, 8, 0.0, false);
  if (state != RADIOLIB_ERR_NONE) {
    printf("Initialization failed, code %d\n", state);
    return 1;
  }
  printf("Initialization success!\n");

  // Open the file
  std::ifstream file("test_tx.txt");
  if (!file.is_open()) {
    printf("Failed to open file!\n");
    return 1;
  }

  std::string line;
  // Read and transmit each line from the file
  while (std::getline(file, line)) {
    printf("[SX1262] Transmitting: %s\n", line.c_str());
    state = radio.transmit(line.c_str());
    if (state == RADIOLIB_ERR_NONE) {
      printf("Transmission success!\n");
    } else {
      printf("Transmission failed, code %d\n", state);
    }

    // Wait a delay before transmitting the next line
    hal->delay(1000); // 1000 milliseconds delay
  }

  file.close();
  return 0;
}
