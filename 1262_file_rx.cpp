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
  // Initialize the radio module with XTAL configuration
  printf("[SX1262] Initializing ... ");
  int state = radio.begin(915.0, 125.0, 7, 5, 0, 10, 8, 0.0, false);
  if (state != RADIOLIB_ERR_NONE) {
    printf("Initialization failed, code %d\n", state);
    return 1;
  }
  printf("Initialization success!\n");

  // Start receiving packets
  printf("[SX1262] Starting receiver ... ");
  state = radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
  if (state != RADIOLIB_ERR_NONE) {
    printf("Start receive failed, code %d\n", state);
    return 1;
  }
  printf("Receiver started!\n");

  // Open file for logging received data
  std::ofstream outfile("rx_data.txt", std::ios::out);
  if (!outfile.is_open()) {
    printf("Failed to open file for writing\n");
    return 1;
  }

  int packet_count = 0;

  // Disable terminal buffering for key press detection
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  // Main loop
  while (true) {
    // Check if a packet has been received
    if (radio.getIrqStatus() & RADIOLIB_SX126X_IRQ_RX_DONE) {
      // Read the received packet
      uint8_t data[64];
      size_t len = sizeof(data);
      state = radio.readData(data, len);
      if (state == RADIOLIB_ERR_NONE) {
        // Successfully read the data
        packet_count++;
        printf("Received packet #%d: %s\n", packet_count, data);

        // Write received packet to the file
        outfile << "Packet #" << packet_count << ": " << data << std::endl;
      } else {
        printf("Failed to read data, code %d\n", state);
      }

      // Restart the receiver
      state = radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
      if (state != RADIOLIB_ERR_NONE) {
        printf("Restart receive failed, code %d\n", state);
        break;
      }
    }

    // Check if space bar is pressed to exit the loop
    if (std::cin.peek() == ' ') {
      printf("Exiting...\n");
      break;
    }

    // Small delay to avoid busy waiting
    hal->delay(100);
  }

  // Restore terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  // Close the file
  outfile.close();

  // Print the final packet count
  printf("Total packets received: %d\n", packet_count);

  return 0;
}
