#!/usr/bin/python3

import cgi
import cgitb
import datetime

# Enable error reporting
cgitb.enable()

print("<html>")
print("<body>")
print("<p>The current time is: {}</p>".format(datetime.datetime.now()))
print("</body>")
print("</html>")
