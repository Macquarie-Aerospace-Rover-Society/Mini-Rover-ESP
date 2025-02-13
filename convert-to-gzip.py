import gzip
import sys

def compress_file(input_file, output_file):
    """Compress a file into Gzip format."""
    try:
        with open(input_file, 'rb') as f_in, gzip.open(output_file, 'wb') as f_out:
            f_out.writelines(f_in)
        print(f"File '{input_file}' compressed to '{output_file}' successfully.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python compress.py <input_file> <output_file.gz>")
    else:
        compress_file(sys.argv[1], sys.argv[2])
