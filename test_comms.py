# import serial
# import serial.tools.list_ports
# import time
# import threading
# import keyboard  # You need to install the keyboard module

# # List all available COM ports
# ports = list(serial.tools.list_ports.comports())
# print("Available COM ports:")
# for port in ports:
#     print(port)

# # Configure the serial port
# try:
#     ser = serial.Serial('COM3', 115200)  # Replace 'COM3' with your Arduino's COM port
#     time.sleep(2)  # Wait for the serial connection to initialize
#     print("Serial port opened successfully.")
# except serial.SerialException as e:
#     print(f"Error opening serial port: {e}")
#     exit()

# # Flags to control the sending and receiving process
# stop_sending = threading.Event()
# stop_receiving = threading.Event()

# def send_file():
#     try:
#         with open('tx_file.txt', 'r') as file:
#             print("Send file opened successfully.")
#             for line in file:
#                 if stop_sending.is_set() or stop_receiving.is_set():
#                     print("Stopping send thread...")
#                     break
#                 line = line.strip()
#                 print(f"Sending: {line}")  # Print the data being sent to the console
#                 ser.write((line + '\n').encode('utf-8'))  # Send the data over the serial port
#                 time.sleep(1)  # Add a delay to ensure the data is sent properly
#     except Exception as e:
#         print(f"Error reading from file: {e}")
#         stop_receiving.set()  # Signal the receiving thread to stop
#     except KeyboardInterrupt:
#         print("Keyboard interrupt received. Exiting send thread...")

# def receive_file():
#     try:
#         with open('rx_file.txt', 'a') as file:
#             print("Receive file opened successfully.")
#             while not stop_receiving.is_set():
#                 if ser.in_waiting > 0:
#                     line = ser.readline().decode('utf-8').strip()
#                     print(f"Received: {line}")  # Print the received data to the console
#                     file.write(line + '\n')  # Write the received data to the file
#                     file.flush()  # Ensure data is written to the file
#                 else:
#                     print("No data waiting in serial buffer.")
#                     time.sleep(1)  # Add a small delay to avoid busy-waiting
#     except Exception as e:
#         print(f"Error writing to file: {e}")
#         stop_sending.set()  # Signal the sending thread to stop
#     except KeyboardInterrupt:
#         print("Keyboard interrupt received. Exiting receive thread...")

# # Create threads for sending and receiving
# send_thread = threading.Thread(target=send_file)
# receive_thread = threading.Thread(target=receive_file)

# # Start the threads
# send_thread.start()
# receive_thread.start()

# # Wait for button press to stop sending
# print("Press 'Ctrl + Z' to stop sending.")
# keyboard.wait('ctrl+z')
# stop_sending.set()
# stop_receiving.set()

# # Wait for both threads to complete
# send_thread.join()
# receive_thread.join()

# # Close the serial port
# ser.close()
# print("Serial port closed.")

# import serial
# import time
# import signal
# import sys
# import argparse

# def send_file(file_path):
#     try:
#         with open(file_path, 'r') as file:
#             for line in file:
#                 ser.write(line.encode('utf-8'))
#                 print(f"Sent: {line.strip()}")
#                 time.sleep(2)  # Delay between sending lines
#     except Exception as e:
#         print(f"Error: {e}")

# def receive_data():
#     try:
#         with open('rx_file.txt', 'w') as file:
#             while True:
#                 if ser.in_waiting > 0:
#                     line = ser.readline().decode('utf-8').strip()
#                     print(f"Received: {line}")
#                     file.write(line + '\n')
#                 else:
#                     time.sleep(1)  # Add a small delay to avoid busy-waiting
#     except KeyboardInterrupt:
#         print("Keyboard interrupt received. Exiting...")
#     finally:
#         ser.close()

# def handle_sigint(signum, frame):
#     print("SIGINT received. Exiting...")
#     ser.close()
#     sys.exit(0)

# if __name__ == "__main__":
#     # Register the SIGINT signal handler
#     signal.signal(signal.SIGINT, handle_sigint)

#     # Parse command-line arguments
#     parser = argparse.ArgumentParser(description='Send and receive data over serial.')
#     parser.add_argument('input_file', help='Path to the input file to send')
#     args = parser.parse_args()

#     # Configure the serial port
#     try:
#         ser = serial.Serial('COM4', 9600, timeout=1)  # Replace 'COM4' with your serial port
#     except serial.SerialException as e:
#         print(f"Could not open serial port: {e}")
#         sys.exit(1)

#     # Send and receive data
#     try:
#         send_file(args.input_file)
#         receive_data()
#     except KeyboardInterrupt:
#         print("Keyboard interrupt received. Exiting...")
#     finally:
#         ser.close()

import serial
import time
import signal
import sys
import argparse

def send_file(file_path):
    try:
        with open(file_path, 'r') as file:
            for line in file:
                ser.write(line.encode('utf-8'))
                print(f"Sent: {line.strip()}")
                time.sleep(2)  # Delay between sending lines
    except Exception as e:
        print(f"Error: {e}")

def receive_data():
    try:
        with open('rx_file.txt', 'w') as file:
            while True:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    print(line)  # Print raw data
                    file.write(line + '\n')  # Write raw data to file
                else:
                    time.sleep(1)  # Add a small delay to avoid busy-waiting
    except KeyboardInterrupt:
        print("Keyboard interrupt received. Exiting...")
    finally:
        ser.close()

def handle_sigint(signum, frame):
    print("SIGINT received. Exiting...")
    ser.close()
    sys.exit(0)

if __name__ == "__main__":
    # Register the SIGINT signal handler
    signal.signal(signal.SIGINT, handle_sigint)

    # Parse command-line arguments
    parser = argparse.ArgumentParser(description='Send and receive data over serial.')
    parser.add_argument('input_file', help='Path to the input file to send')
    args = parser.parse_args()

    # Configure the serial port
    try:
        ser = serial.Serial('COM4', 9600, timeout=1)  # Replace 'COM4' with your serial port
    except serial.SerialException as e:
        print(f"Could not open serial port: {e}")
        sys.exit(1)

    # Send and receive data
    try:
        send_file(args.input_file)
        receive_data()
    except KeyboardInterrupt:
        print("Keyboard interrupt received. Exiting...")
    finally:
        ser.close()