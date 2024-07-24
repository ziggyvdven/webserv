#! /usr/bin/env python3

import os

print(f"""
<h1>CGI: echo.py</h1>
<h4>Welcom {os.environ.get("PATH_INFO")}</h4>
""")