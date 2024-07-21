#! /usr/bin/env python3
from http.server import HTTPServer, BaseHTTPRequestHandler

class MyHttpHandler(BaseHTTPRequestHandler):
	def do_POST(self):
		
	def do_GET(self):
		self.do_POST()

	def get_body(self):
		content_len = int(self.headers.get('Content-Length', 0))
		body = self.rfile.read(content_len)
		return body

def run(server_class=HTTPServer, handler_class=BaseHTTPRequestHandler):
	server_address = ('', 8000)
	httpd = server_class(server_address, handler_class)
	httpd.handle_request()
	print("Request received!!!")

import os
import cgi, cgitb

cgitb.enable()

def parse_multi_part():
	form = cgi.FieldStorage()
	print("[Debug]", "fileitem", form['fileitem'])


def main():
	run(handler_class=MyHttpHandler)


if __name__ == "__main__":
	main()
