#!/usr/bin/env python3

# Upload File Script

import os
import sys

# print (os.getcwd())

if os.getenv('METHOD') == 'POST':

	file_path = os.getenv('FILENAME')
	content_length = int(os.getenv('CONTENT_LENGTH'))

	s = sys.stdin.buffer.read(content_length)

	f = open(file_path, "wb")
	f.write(s)
	f.close()

exit (0)

# print ("Hello World !")
# exit (0)