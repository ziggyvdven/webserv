#! /usr/bin/env python3

import sys, cgi
print("Hello from echo.py\n")

form = cgi.FieldStorage()
print(f"{form=}")

print("Cgi OUT")