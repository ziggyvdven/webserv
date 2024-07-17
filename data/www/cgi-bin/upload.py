#!/usr/bin/env python3

# Upload File Script

import os
import sys

# print (os.getcwd())

if os.getenv('METHOD') == 'POST':

	file_path = os.getenv('FILENAME')
	content_length = int(os.getenv('CONTENT_LENGTH'))

	dir = os.path.dirname(file_path)
	if (not os.path.isdir(dir)):
		os.makedirs(dir)

	s = sys.stdin.buffer.read(content_length)

	# print ("Before open")
	f = open(file_path, "wb")
	f.write(s)
	f.close()
	# print ("End")

exit (0)