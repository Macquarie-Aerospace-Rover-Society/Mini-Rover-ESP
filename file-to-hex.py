def file_to_hex(input_file, output_file):
    """Convert a file into a hex byte array format and save it."""
    try:
        with open(input_file, 'rb') as f:
            byte_data = f.read()
        
        hex_lines = []
        for i in range(0, len(byte_data), 16):
            hex_chunk = ', '.join(f'0x{b:02X}' for b in byte_data[i:i+16])
            hex_lines.append(hex_chunk)
        
        hex_string = ',\n'.join(hex_lines)
        
        with open(output_file, 'w') as f:
            f.write(hex_string)
        
        print(f"File '{input_file}' converted to hex format and saved to '{output_file}'.")
        print(f"Total bytes: {len(byte_data)}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python file_to_hex.py <input_file> <output_file>")
    else:
        file_to_hex(sys.argv[1], sys.argv[2])
