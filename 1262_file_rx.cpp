#include <iostream>
#include <fstream>
#include <string>
#include <RadioLib.h>
#include "PiHal.h"
#include <termios.h>
#include <unistd.h>
#include <chrono>
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
        // Init with current values
        printf("[SX1262] Initializing ... \n");
        state = radio.begin(915.0, BW[bw_idx], SF[sf_idx], CR[cr_idx], 18, 10, 8, 0.0, false);
        if (state != RADIOLIB_ERR_NONE) {
          printf("Initialization failed, code %d\n", state);
          return 1;
        }
        printf("Initialization success for SF=%d, BW=%.1f, CR=%d\n", SF[sf_idx], BW[bw_idx], CR[cr_idx]);
        // Start receiving
        printf("[SX1262] Starting receiver ... ");
        state = radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
        if (state != RADIOLIB_ERR_NONE) {
          printf("Start receive failed, code %d\n", state);
          return 1;
        }
        printf("Receiver started!\n");
        packet_count = 0;
        // Main waiting loop -> space breaks us out onto next file
        while (true) {
          uint8_t data[64];  // Buffer to hold received data
          size_t len = sizeof(data);  // Length of the buffer
          state = radio.readData(data, len);
          if (state == RADIOLIB_ERR_NONE) {
            std::string str(reinterpret_cast<char*>(data), len);
            printf("Received packet: %s\n", str.c_str());
            outfile << str << std::endl;
            packet_count++;
          } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
            // No packet received, continue waiting
            printf("No packet received, continuing...\n");
            continue;
          } else {
            printf("Receive failed, code %d\n", state);
            break;
          }
          // Check for space bar press to break out of the loop
          if (getchar() == ' ') {
            break;
          }
        }
        // Close the file after writing
        outfile.close();
        printf("File %s closed successfully with %d packets received\n", filename.c_str(), packet_count);
      }
    }
  }
  // Restore terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return 0;
}