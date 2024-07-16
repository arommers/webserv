#!/usr/bin/python3

import os
import sys
import cgi
import cgitb


# input_data = sys.stdin.read()

# print("<p>{}</p>".format(input_data))
# print("Hello! This is the upload script\n");

# for key, value in os.environ.items():
#     print(f"{key}:{value}")
cgitb.enable()

# input_data = sys.stdin.read(297)

# # Print the raw input data to stderr for debugging
# print("Raw input data:", input_data, file=sys.stderr)
print("Environment variables:", os.environ, file=sys.stderr)
form = cgi.FieldStorage()

if not form:
    print("Form not working\n")
for field in form.keys():
    item = form[field]
    if item.filename:  # This is a file upload field
        print(f"<h2>Field: {field} (File)</h2>")
        print(f"<p>Filename: {item.filename}</p>")
        print("<pre>")
        print(item.file.read().decode(errors='replace'))
        print("</pre>")
    else:  # This is a regular form field
        print(f"<h2>Field: {field}</h2>")
        print(f"<p>Value: {item.value}</p>")

# print("""
# <!DOCTYPE html>
# <html lang="en">
# <head>
#     <meta charset="UTF-8">
#     <meta name="viewport" content="width=device-width, initial-scale=1.0">
#     <title>CGI Script Success</title>
# </head>
# <body>
#     <h1>Upload CGI script successfully run!</h1>
# </body>
# </html>
# """)

print ("Script test");