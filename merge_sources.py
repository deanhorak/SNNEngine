#!/usr/bin/env python3

import os

def main():
    # Specify the output file
    output_filename = "FullProject.txt"

    # Optionally, remove the old output file if it exists,
    # so we start fresh each time:
    if os.path.exists(output_filename):
        os.remove(output_filename)

    # Collect all .cpp and .h files in the current directory
    source_files = [f for f in os.listdir('.') if f.endswith('.cpp') or f.endswith('.h')]

    # Sort them if you want a predictable order
    source_files.sort()

    # Open the output file in write mode
    with open(output_filename, 'w') as outfile:
        for filename in source_files:
            # Write a separator or comment header for clarity
            outfile.write(f"// ============== {filename} ==============\n")
            # Read the file and concatenate its contents
            with open(filename, 'r') as infile:
                outfile.write(infile.read())
            outfile.write("\n\n")  # Blank line after each file

    print(f"Merged {len(source_files)} files into {output_filename}")

if __name__ == "__main__":
    main()
