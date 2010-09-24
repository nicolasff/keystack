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
		self.s.send(pkt)
		return self.parse(self.s.recv(1) + self.s.recv(1024))

	def get(self, key):
		pkt = self.fmtString("\x01" + self.fmtString(key))
		self.s.send(pkt)
		return self.parse(self.s.recv(1) + self.s.recv(1024))


	def parse(self, pkt):
		# print [ord(c) for c in pkt]
		if pkt[0] == "\x00":
			if pkt[1] == "\x00":
				return False
			elif pkt[1] == "\x01":
				return True
		elif pkt[0] == "\x01":
			(sz,) = struct.unpack('>l', pkt[1:5])
			s = pkt[5:5+sz]
			return (sz, s)

c = Client()
c.connect('127.0.0.1', 1277)
for i in range(1, 1000):
	k = "key-%d" % i
	v = "val-%d" % i
	print c.set(k, v)
	print c.get(k)
	print
