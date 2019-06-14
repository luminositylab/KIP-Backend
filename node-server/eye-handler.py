#!/usr/bin/env python

import sys

def hello(variable):
    print("from python: ", variable)

data = sys.stdin.read()
hello(data)
