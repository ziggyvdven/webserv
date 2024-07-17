#!/usr/bin/env python3

import os
import sys

print ("Start")

str = sys.stdin.read(int(os.getenv('CONTENT_LENGTH')))

print (str)

print ("Finish!")