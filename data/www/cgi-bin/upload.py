#!/usr/bin/env python3

# Upload File Script


import os
import sys

def getenv(varname, vartype=str):
	value = os.getenv(varname)
	if value is not None:
		try:
			return vartype(value)
		except:
			raise ValueError(f"Cannot cast environment variable {value} to {vartype}")
	return None

def read_stdin():
	content_length = getenv('CONTENT_LENGTH', int)
	s = sys.stdin.buffer.read(content_length)
	if not s:
		sys.stdout.write(f"<h1>{content_length} bytes Uploaded Successful</h1>")
		exit(0)
	return s

def get_boundary(content_type):
	i = content_type.find("=")
	return content_type[(i + 1)::]

def is_multi_part(content_type):
	return content_type.startswith("multipart/form-data;")

def write_files():
	pass

def print_response():
	pass


def stdout(str):
	sys.stdout.write(str)

# with open(file_path, "wb") as f:
	# f.write(s)


def main():
	if os.getenv('METHOD') != 'POST':
		stdout("<h1>[DEBUG] Upload Failed: Not POST request</h1>")
		exit(0)

	if not is_multi_part():
		stdout("<h1>[DEBUG] Upload Failed: Not Multi Part</h1>")
		exit(0)

	# Gen env variables
	file_path = getenv('FILENAME')
	content_type = getenv('CONTENT_TYPE', str)

	# Create folder if it does not exit
	dir = os.path.dirname(file_path)
	if (not os.path.isdir(dir)):
		os.makedirs(dir)

	request_body = read_stdin()

	files_struct = parse_body(request_body)

	print(f"{content_type=}")
	print("")

	exit (0)

if __name__ == "__main__":
	main()