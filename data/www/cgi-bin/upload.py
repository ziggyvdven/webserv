#!/usr/bin/env python3

# Upload File Script


import os
import sys
import cgi, cgitb

cgitb.enable()

def response_headers():
	print("Content-Type: text/html")
	print()

def upload_file(field_item, upload_dir="./data/www/upload/"):
	file_name = os.path.basename(field_item.filename)
	file_path = os.path.join(upload_dir, file_name)
	count = 0
	with open(file_path, 'wb') as file:
		while True:
			count += 1
			chunk = field_item.file.read(500000)
			if not chunk:
				break
			file.write(chunk)
	print(f"</br>- {count} chunk(s) Wrote to {file_path}</br>")
	return file_path

def main():

	response_headers()

	# Populate form cgi data
	form = cgi.FieldStorage()
	
	if not form:
		print("<h1>Upload: No files found!</h1>")
		exit(0)

	# Create folder if it does not exit
	for key in form.keys():
		field_item = form[key]
		if field_item.filename:
			upload_file(field_item)
		else:
			print(f"Field: {key}, Value: {field_item.value}")
		print("</br>")
	
	print("<h1>Upload successful!</h1>")
	exit (0)

if __name__ == "__main__":
	main()