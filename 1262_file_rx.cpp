#include <iostream>
#include <fstream>
#include <RadioLib.h>
#include "PiHal.h"
#include <termios.h>
#include <unistd.h>

// Create a new instance of the HAL class
PiHal* hal = new PiHal(0);

// Create the radio module instance
// NSS pin: WiringPi 10 (GPIO 8)
// DIO1 pin: WiringPi 2 (GPIO 27)
// NRST pin: WiringPi 21 (GPIO 5)
// BUSY pin: WiringPi 0 (GPIO 17)
SX1262 radio = new Module(hal, 10, 2, 21, 0);

int main() {
  // Test params
  float BW[] = {62.5, 125.0, 250.0};
  int SF[] = {7, 9, 11};
  int CR[] = {5, 8};

  int state;
  int packet_count;
  std::ofstream outfile;

  // Space bar for test advancing
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  // Initialize the radio module with XTAL configuration
  printf("[SX1262] Initializing ... ");
  state = radio.begin(915.0, 125.0, 7, 5, 0, 10, 8, 0.0, false);
  if (state != RADIOLIB_ERR_NONE) {
    printf("Initialization failed, code %d\n", state);
    return 1;
  }
  printf("Initialization success!\n");

  // Loop through the BW, SF, and CR values
  for (int cr_idx = 0; cr_idx < 2; cr_idx++) {
    for (int sf_idx = 0; sf_idx < 3; sf_idx++) {
      for (int bw_idx = 0; bw_idx < 3; bw_idx++) {

        // Make file name
        std::string filename = "rx_data_" + std::to_string(SF[sf_idx]) + "_" + std::to_string((int)BW[bw_idx]) + "_" + std::to_string(CR[cr_idx]) + ".txt";
        outfile.open(filename);
        if (!outfile.is_open()) {
          printf("Failed to open file %s for writing\n", filename.c_str());
          return 1;
        }
        printf("File %s opened successfully for writing\n", filename.c_str());

        // Add code to receive data and write to file
        // Example:
        // std::string receivedData = radio.receive();
        // outfile << receivedData << std::endl;

        // Close the file after writing
        outfile.close();
        printf("File %s closed successfully\n", filename.c_str());
      }
    }
  }

  // Restore terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return 0;
}