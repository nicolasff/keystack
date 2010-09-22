#!/usr/bin/python
from socket import *
import struct

s = socket(AF_INET, SOCK_STREAM, 0)
s.connect(('127.0.0.1', 1277))

def display(pkt):
	# print [ord(c) for c in pkt]
	if pkt[0] == "\x00":
		if pkt[1] == "\x00":
			print "[BOOL] FALSE"
		elif pkt[1] == "\x01":
			print "[BOOL] TRUE"
	elif pkt[0] == "\x01":
		(sz,) = struct.unpack('>l', pkt[1:5])
		s = pkt[5:5+sz]

		print "[STRING (len=%d)] \"%s\"" % (sz, s)




print "SET(hello, world)"
s.send("\x00\x00\x00\x13" + "\x02" + "\x00\x00\x00\x05" + "hello" + "\x00\x00\x00\x05" + "world")
pkt = s.recv(1) + s.recv(1024)
display(pkt)

print "GET(hello)"
s.send("\x00\x00\x00\x0A" + "\x01" + "\x00\x00\x00\x05" + "hello")
pkt = s.recv(1) + s.recv(1024)
display(pkt)
