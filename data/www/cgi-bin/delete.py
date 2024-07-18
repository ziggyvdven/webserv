#!/usr/bin/python3

# Upload File Script

import os

# print (os.getcwd())

if os.getenv('METHOD') == 'DELETE':

	file_path = os.getenv('FILENAME')

	f = open(file_path, "wb")
	f.write(os.getenv('CONTENT'))
	f.close()

exit (0)
