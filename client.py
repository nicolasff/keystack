#!/usr/bin/python
from socket import *
import struct


class Client:
	def __init__(self):
		self.s = socket(AF_INET, SOCK_STREAM, 0)
		self.fmt32 = '>l'
		
	def connect(self, host, port):
		return self.s.connect((host, port))

	def fmt32(self, l):
		return struct.pack(">l", l)

	def fmtString(self, s):
		return struct.pack(">l", len(s)) + s

	def set(self, key, val):
		pkt = self.fmtString("\x02" + self.fmtString(key) + self.fmtString(val))
		return self.run(pkt)

	def get(self, key):
		pkt = self.fmtString("\x01" + self.fmtString(key))
		return self.run(pkt)

	def run(self, pkt):
		self.s.send(pkt)
		cmdType = self.s.recv(1)

		if cmdType == "\x00":
			b = self.s.recv(1)
			return (b == "\x01")

		elif cmdType == "\x01":
			szStr = self.s.recv(4)
			(sz,) = struct.unpack('>l', szStr)
			s = self.s.recv(sz)
			return (sz, s)

c = Client()
c.connect('127.0.0.1', 1277)
for i in range(1, 1000):
	k = "key-%d" % i
	v = "val-%d" % i
	print c.set(k, v)
	print c.get(k)
	print
