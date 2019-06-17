#!/usr/bin/env python

import sys

def hello(variable):
    print(variable)

data = sys.stdin.read()
hello(data)
