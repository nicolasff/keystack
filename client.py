from socket import *

s = socket(AF_INET, SOCK_STREAM, 0)
s.connect(('127.0.0.1', 1277))

s.send("\x00\x00\x00\x13" + "\x02" + "\x00\x00\x00\x05" + "hello" + "\x00\x00\x00\x05" + "world")
print s.recv(1024)

s.send("\x00\x00\x00\x0B" + "\x01" + "\x00\x00\x00\x05" + "hello")
print s.recv(1024)
