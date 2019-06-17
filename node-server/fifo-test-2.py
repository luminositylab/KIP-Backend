import os
import sys

path = "./myfifo"
fifo = open(path, "r")
for line in fifo:
    print("Received: " + line)
fifo.close()
