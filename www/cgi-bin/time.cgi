#!/usr/bin/python3

import cgi
import cgitb
import datetime

# Enable error reporting
cgitb.enable()

print("<html>")
print("<head>")
print("<title>Current Time</title>")
print("</head>")
print("<body>")
print("<h1>Current Time</h1>")
print("<p>The current time is: {}</p>".format(datetime.datetime.now()))
print("</body>")
print("</html>")
