#!/usr/bin/env python3

# Upload File Script

import os
import sys

if os.getenv('METHOD') != 'POST':
	exit(0)

# Gen env variables
file_path = os.getenv('FILENAME')
content_length = int(os.getenv('CONTENT_LENGTH'))

print("Hello World")

exit(0)

# Create folder if it does not exit
dir = os.path.dirname(file_path)
if (not os.path.isdir(dir)):
	os.makedirs(dir)

s = sys.stdin.buffer.read(content_length)

# Write to file
with open(file_path, "wb") as f:
	f.write(s)

exit (0)