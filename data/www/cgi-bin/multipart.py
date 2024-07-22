#! /usr/bin/env python3
from http.server import HTTPServer, BaseHTTPRequestHandler


import os, sys
import cgi, cgitb

# cgitb.enable()

def parse_multi_part():
	form = cgi.FieldStorage()
	# print("[Debug]", "fileitem", form.get('fileitem', None))
	print("[Debug]", "form:\n", form)
	return form

def print_env():
	vars = {
		'REQUEST_METHOD': os.environ.get('REQUEST_METHOD'),
		'CONTENT_TYPE': os.environ.get('CONTENT_TYPE'),
		'CONTENT_LENGTH': os.environ.get('CONTENT_LENGTH', '0')
	}
	print(vars)


def dump_stdin():
	content = sys.stdin.read()
	print(content)

def main():
	# print_env()
	# dump_stdin()
	form = parse_multi_part()

	print("Content-Type: text/plain")
	print()
	print(f"{form=}")



if __name__ == "__main__":
	main()
