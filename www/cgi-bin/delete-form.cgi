#!/usr/bin/env python3

import cgi
import json
import sys


form_data = sys.stdin.read()

line = form_data[5:].strip()

# Load existing data
try:
    with open('www/html/form_data.json', 'r') as file:
        data = json.load(file)
except FileNotFoundError:
    data = []

# Find the entry with the given ID and remove it
data = [entry for entry in data if entry['id'] != line]

# Save the updated data back to the file
with open('www/html/form_data.json', 'w') as file:
    json.dump(data, file, indent=4)

# Output a success message
print("<html><body>")
print("<h1>Entry Deleted</h1>")
print("</body></html>")
