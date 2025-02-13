import gzip

# Decompress the extracted file
with gzip.open("compressed_data.gz", "rb") as f:
    decompressed_data = f.read()

# Convert binary to readable text
decoded_text = decompressed_data.decode(errors="ignore")  # Ignore non-text bytes

# Print or save the output
print(decoded_text)

with open("output.html", "w") as f:
    f.write(decoded_text)