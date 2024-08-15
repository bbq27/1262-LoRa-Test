#calculate_ber.py
#HOW TO RUN: "python calculate_ber.py tx_file.txt rx_file.txt" through CLI
#First file is the reference, second file is the received data
import argparse

def calculate_ber(transmitted_file, received_file):
    with open(transmitted_file, 'r') as tx_file, open(received_file, 'r') as rx_file:
        tx_lines = tx_file.readlines()
        rx_lines = rx_file.readlines()

    # Ensure both files have the same number of lines
    if len(tx_lines) != len(rx_lines):
        raise ValueError("Transmitted and received files have different numbers of lines.")

    total_bits = 0
    error_bits = 0

    for tx_line, rx_line in zip(tx_lines, rx_lines):
        tx_line = tx_line.strip()
        rx_line = rx_line.strip()

        # Ensure both lines have the same length
        if len(tx_line) != len(rx_line):
            raise ValueError("Mismatched line lengths in transmitted and received files.")

        total_bits += len(tx_line)
        for tx_bit, rx_bit in zip(tx_line, rx_line):
            if tx_bit != rx_bit:
                error_bits += 1

    ber = error_bits / total_bits if total_bits > 0 else 0
    return ber

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Calculate Bit Error Rate (BER).')
    parser.add_argument('transmitted_file', type=str, help='Path to the transmitted file')
    parser.add_argument('received_file', type=str, help='Path to the received file')

    args = parser.parse_args()

    ber = calculate_ber(args.transmitted_file, args.received_file)
    print(f"Bit Error Rate (BER): {ber:.10f}")
