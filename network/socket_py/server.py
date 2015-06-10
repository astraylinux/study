#!/usr/bin/env python
# encoding: utf-8
"""
	study python socket program. I must to fix the shadowsocks
"""
import socket
import commands
import json

BUFF_SIZE = 64 * 1024
END_FLAG = "#@Done.@#"

class CommandServer(object):
	"""
		Create a socket, accept client send command line.
		Run command, and send the output.
	"""
	def __init__(self, host, port):
		self._host = host
		self._port = port
		self._socket = None

	def connect(self):
		"""
			Connect to self host and port, init _socket.
			Bind the host and port, waiting connected.
		"""
		self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self._socket.bind((self._host, self._port))
		self._socket.listen(5)
		return self._socket

	def sendall(self, conn, data):
		conn.sendall(data)
		conn.sendall(END_FLAG)

	def run(self):
		""" start server """
		self.connect()
		if not self._socket:
			print "Create server failed."
			exit()
		print "Server running..."
		while True:
			(conn, addr) = self._socket.accept()
			print 'Connected by', addr
			while True:
				cmd = conn.recv(BUFF_SIZE)
				if cmd == "":
					conn.sendall(END_FLAG)
					continue
				print "RECV:", cmd
				if cmd == "exit":
					print addr, "exit!"
					self.sendall(conn, "Bye.")
					break
				(cstatus, coutput) = commands.getstatusoutput(cmd)
				send_back = {"status":cstatus, "output":coutput}
				self.sendall(conn, json.dumps(send_back, ensure_ascii=False))
			conn.close()







