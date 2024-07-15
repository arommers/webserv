#!/usr/bin/python3

import os

print("Hello! This is the upload script\n");

for key, value in os.environ.items():
    print(f"{key}:{value}")