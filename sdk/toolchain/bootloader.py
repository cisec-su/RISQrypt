import serial
import time
import argparse
from tqdm import tqdm


def send_file_via_uart(ser, file_path, chunk_size=1024, sleep_time=0.01):
    with open(file_path, "rb") as file:
        print(f"Sending file: {file_path}")

        file_size = file.seek(0, 2)  # Move the cursor to the end of the file
        file.seek(0)  # Move the cursor to the start of the file
        with tqdm(total=file_size, unit='B', unit_scale=True, desc=file_path) as pbar:
            while chunk := file.read(chunk_size):
                ser.write(chunk)
                time.sleep(sleep_time)  # Adjust this delay as needed
                pbar.update(len(chunk))
            
    print("File transmission complete.")
    
        
def send_data(ser, data):
    if ser.is_open:
        ser.write(data.encode('utf-8'))
        print(f"Sent: {data}")

def read_data(ser):
    if ser.is_open:
        incoming_data = ser.readline().decode('utf-8').strip()
        if incoming_data:
            print(f"Received: {incoming_data}")
        else:
            print("No data received.")

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Hornet Programmer")
    parser.add_argument(
            "-p", "--port",
            type=str,
            required=False,
            default='/dev/ttyUSB1',
            help="Serial Port"
        )
    parser.add_argument(
            "-b", "--baudrate",
            type=int,
            required=False,
            default=9600,
            help="Baudrate"
        )
    parser.add_argument(
            "-f", "--file",
            type=str,
            required=False,
            default='../examples/uart_example/uart_example.bin',
            help="File to send"
        )
    parser.add_argument(
            "-c", "--chunk_size",
            type=int,
            required=False,
            default=1024,
            help="Chunk size"
        )
    parser.add_argument(
            "-s", "--sleep_time",
            type=float,
            required=False,
            default=0.01,
            help="Sleep time"
        )
    parser.add_argument(
            "-t", "--timeout",
            type=float,
            required=False,
            default=1,
            help="Timeout"
        )

    args = parser.parse_args()

    ser = serial.Serial(args.port, baudrate=args.baudrate, timeout=args.timeout)

    if ser.is_open:
        print(f"Successfully connected to {ser.name}")
    else:
        print(f"Error opening serial port {ser.name} {ser.port}")
        exit(1)


    try:
        time.sleep(1)
        send_data(ser, "-p")
        time.sleep(1)
        read_data(ser)
        send_file_via_uart(ser, args.file, chunk_size=args.chunk_size, sleep_time=args.sleep_time)

        while(True):
            read_data(ser)


        
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        if ser.is_open:
            ser.close()
            print("Serial port closed.")