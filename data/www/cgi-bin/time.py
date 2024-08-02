#!/usr/bin/env python3

import os
import time

def response(current_time):

	header = os.getenv('ROOT') + '/template/header.html'
	footer = os.getenv('ROOT') + '/template/footer.html'

	with open(header, 'r') as f:
		print(f.read())
	f.close()
	print('<h1>Current Time</h1>')
	print('<div class="content_small">')
	print(current_time)
	print('</div>')
	with open(footer, 'r') as f:
		print(f.read())
	f.close()


def main():

	t = time.localtime()
	current_time = time.strftime('%H:%M:%S', t)
	response(current_time)
	exit (0)

if __name__ == "__main__":
	main()
