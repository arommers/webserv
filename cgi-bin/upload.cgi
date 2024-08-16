#!/usr/bin/python3

import os
import sys
import cgi
import io
import cgitb


cgitb.enable()

buffer_size = int(os.environ.get('BUFFER_SIZE'))
received_data = []

while True:
    chunk = sys.stdin.read(buffer_size)
    received_data.append(chunk)
    if not chunk:
        break

input_data = ''.join(received_data)
input_stream = io.BytesIO(input_data.encode())

form = cgi.FieldStorage(fp=input_stream, environ=os.environ, keep_blank_values=True)


fileitem = form['fileToUpload']

if fileitem.filename:
    
    fn = os.path.basename(fileitem.filename)
    savePath = os.path.join("./uploads/", fn)

    try:
        os.makedirs(os.path.dirname(savePath), exist_ok=True)

        with open(savePath, 'wb') as f:
            f.write(fileitem.file.read())
        
        print(f"<h1>File {fn} uploaded successfully!</h1>")
    
    except Exception as e:
        print("<h1>Error saving file</h1>")
        print(f"<p>{e}</p>")
else:
    print("<h1>No file was uploaded</h1>")
