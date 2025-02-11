# generate_header.py
import sys

def generate_header(input_filename, output_filename, const_name):
    with open(input_filename, 'r') as infile, open(output_filename, 'w') as outfile:
        content = infile.read()
        name = const_name.upper()
        outfile.write(f'#ifndef MARS_WIFI_{name}\n#define MARS_WIFI_{name}\n\n#include <pgmspace.h>\n\nconst char HTML_{name}[] PROGMEM = R"rawliteral(\n{content}\n)rawliteral";\n\n#endif')

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <input file> <output file> <NAME>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    const_name = sys.argv[3]
    generate_header(input_file, output_file, const_name)