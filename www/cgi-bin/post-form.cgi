#!/usr/bin/python3

import os
import sys
import cgi
import io
import cgitb
import json
import uuid 

file_path = "www/html/form_data.json"
try:
    if os.path.exists(file_path) and os.path.getsize(file_path) > 0:
        with open(file_path, 'r') as file:
            data = json.load(file)
    else:
        data = []
except json.JSONDecodeError:
    data = []

# Process form data
form = cgi.FieldStorage()
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')
email = form.getvalue('email')

entry_id = str(uuid.uuid4())

# Append new entry to the list
entry = {
    'id': entry_id,
    'first_name': first_name,
    'last_name': last_name,
    'email': email
}
data.append(entry)

# Write updated data back to the file
with open(file_path, 'w') as file:
    json.dump(data, file, indent=4)