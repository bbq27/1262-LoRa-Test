#include <RadioLib.h>
#include <fstream>
#include <string>
#include <csignal>
#include <iostream>
#include <unistd.h> // Include for getcwd
#include <termios.h>
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

volatile bool keepRunning = true;

void signalHandler(int signum) {
  keepRunning = false;
}

int main() {
  // Register signal handler for Ctrl+C
  signal(SIGINT, signalHandler);

  // Initialize the radio module with XTAL configuration
  printf("[SX1262] Initializing ... ");
  int state = radio.begin(915.0, 125.0, 7, 5, 0, 10, 8, 0.0, false);
  if (state != RADIOLIB_ERR_NONE) {
    printf("Initialization failed, code %d\n", state);
    return 1;
  }
  printf("Initialization success!\n");

  // Open the file using an absolute path
  std::ifstream file("C:\\Users\\berfr\\repos\\1262_tests\\test_tx.txt");
  if (!file.is_open()) {
    printf("Failed to open file!\n");
    return 1;
  }
  printf("File opened successfully.\n");

  std::string line;
  // Read and transmit each line from the file
  while (keepRunning && std::getline(file, line)) {
    printf("Read line: %s\n", line.c_str()); // Debugging statement
    printf("[SX1262] Transmitting packet: %s\n", line.c_str());
    state = radio.transmit(line.c_str());
    if (state == RADIOLIB_ERR_NONE) {
      printf("Transmission success!\n");
    } else {
      printf("Transmission failed, code %d\n", state);
    }

    // Wait a delay before transmitting the next line
    hal->delay(1000); // 1000 milliseconds delay

    // Check for user input to stop transmission
    if (std::cin.rdbuf()->in_avail() > 0) {
      char c;
      std::cin >> c;
      if (c == 'q') {
        keepRunning = false;
      }
    }
  }

  printf("Exiting loop.\n"); // Debugging statement
  file.close();
  return 0;
}