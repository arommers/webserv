#!/usr/bin/python3

import cgi
import cgitb
import datetime

print("<html>")
print("<body>")
print("<p>The current time is: {}</p>".format(datetime.datetime.now()))
print("</body>")
print("</html>")
