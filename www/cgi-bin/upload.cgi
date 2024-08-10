#!/usr/bin/python3

import os
import sys
import cgi
import cgitb


cgitb.enable()


form = cgi.FieldStorage()


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
