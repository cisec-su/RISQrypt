import sys
import struct

def binary_to_hex_lines(input_file, output_file, endianness='big'):
    try:
        with open(input_file, 'rb') as bin_file:
            data = bin_file.read()
    except FileNotFoundError:
        print(f"Error: The file {input_file} was not found.")
        return
    except IOError as e:
        print(f"Error reading file {input_file}: {e}")
        return

    # Set the endianness format
    if endianness == 'little':
        endian_format = '<'
    elif endianness == 'big':
        endian_format = '>'
    else:
        print(f"Error: Unsupported endianness '{endianness}'. Use 'little' or 'big'.")
        return

    try:
        with open(output_file, 'w') as hex_file:
            # Process the binary data in chunks of 4 bytes
            for i in range(0, len(data), 4):
                chunk = data[i:i+4]
                # Convert the chunk to the correct endianness
                if len(chunk) == 4:
                    # Use struct to unpack and then pack in the desired endianness
                    unpacked = struct.unpack(endian_format + 'I', chunk)
                    hex_string = f'{unpacked[0]:08X}'
                else:
                    # Handle cases where chunk is less than 4 bytes
                    hex_string = ''.join(f'{byte:02X}' for byte in chunk)
                # Write the hexadecimal string to the output file
                hex_file.write(hex_string + '\n')
    except IOError as e:
        print(f"Error writing to file {output_file}: {e}")
        return

def main():
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    endianness = 'little'
    
    binary_to_hex_lines(input_file, output_file, endianness)

if __name__ == "__main__":
    main()