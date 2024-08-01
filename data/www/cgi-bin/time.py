#!/usr/bin/env python3

import os
import time

def response(current_time):

	with open('./template/header.html', 'r') as f:
		print(f.read())
	f.close()
	print("<h1>Current Time:</h1>")
	print(current_time)
	with open('./template/footer.html', 'r') as f:
		print(f.read())
	f.close()


def main():

	t = time.localtime()
	current_time = time.strftime("%H:%M:%S", t)
	response(current_time)
	exit (0)

if __name__ == "__main__":
	main()
