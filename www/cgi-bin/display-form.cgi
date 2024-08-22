#!/usr/bin/env python3
import json
import os
import sys

sys.stdout.flush()

DATA_FILE = "html/form_data.json"

# Load data from the JSON file
if os.path.exists(DATA_FILE):
    with open(DATA_FILE, "r") as file:
        data = json.load(file)
else:
    data = []

# HTML structure
print("""
<html>
<head><title>Form Data</title></head>
<body>
<h1>Form Data</h1>
<table border="1">
    <thead>
        <tr>
            <th>First Name</th>
            <th>Last Name</th>
            <th>Email Address</th>
            <th>Actions</th>
        </tr>
    </thead>
    <tbody>
""")

# Generate table rows for each entry
for entry in data:
    print(f"""
    <tr>
        <td>{entry['first_name']}</td>
        <td>{entry['last_name']}</td>
        <td>{entry['email']}</td>
        <td>
            <form action="/cgi-bin/delete-form.cgi" method="post" style="display:inline;">
                <input type="hidden" name="id" value="{entry['id']}" />
                <button type="submit">Delete</button>
            </form>
        </td>
    </tr>
    """)

print("""
    </tbody>
</table>
</body>
</html>
""")

