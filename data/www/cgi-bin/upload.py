#!/usr/bin/env python3

# Upload File Script


import os
import sys


def read_stdin():
	content_length = getenv('CONTENT_LENGTH', int)
	s = sys.stdin.buffer.read(content_length)
	if not s:
		sys.stdout.write(f"<h1>{content_length} bytes Uploaded Successful</h1>")
		exit(0)
	return s


def write_files():
	pass

def print_response():
	pass


def stdout(str):
	sys.stdout.write(str)

# with open(file_path, "wb") as f:
	# f.write(s)


import cgi, cgitb

cgitb.enable()

def response_headers():
	print("Content-Type: text/plain")
	print()

def upload_file(field_item, upload_dir="./data/www/upload/"):
	file_name = os.path.basename(field_item.filename)
	file_path = os.path.join(upload_dir, file_name)
	with open(file_path, 'wb') as file:
		while True:
			print(f"Chunk Wrote to {file_path}</br>")
			chunk = field_item.file.read(50000)
			if not chunk:
				break
			file.write(chunk)
	return file_path

def main():

	response_headers()

	# Populate form cgi data
	form = cgi.FieldStorage()
	
	if not form:
		print("No files found!")

	# Create folder if it does not exit
	for key in form.keys():
		field_item = form[key]
		if field_item.filename:
			upload_file(field_item)
		else:
			print(f"Field: {key}, Value: {field_item.value}")
		print("</br>")

	exit (0)

if __name__ == "__main__":
	main()