#! /usr/bin/env python3
from http.server import HTTPServer, BaseHTTPRequestHandler


import os, sys
import cgi, cgitb

# cgitb.enable()

def parse_multi_part():
	form = cgi.FieldStorage()
	# print("[Debug]", "fileitem", form.get('fileitem', None))
	print("[Debug]", "form:\n", form)

def print_env():
	print(os.environ)


def main():
	vars = {
		'REQUEST_METHOD': os.environ.get('METHOD'),
		'CONTENT_TYPE': os.environ.get('CONTENT_TYPE'),
		'CONTENT_LENGTH': os.environ.get('CONTENT_LENGTH', '0')
	}
	print(vars)


if __name__ == "__main__":
	main()
