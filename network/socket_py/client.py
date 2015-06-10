#!/usr/bin/env python
# encoding: utf-8
"""
	study python socket program. I must to fix the shadowsocks
"""
import socket
import json

BUFF_SIZE = 64 * 1024
END_FLAG = "#@Done.@#"

class CommandClient(object):
	"""
		Command client, send command, wait server excute and return
		output.
	"""
	def __init__(self, host, port):
		self._host = host
		self._port = port
		self._socket = None

	def connect(self):
		"""
			init socket, return to outside, it need.
		"""
		self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self._socket.connect((self._host, self._port))
		return self._socket

	def run(self):
		""" start client """
		self.connect()
		while True:
			cmd = raw_input("[not terminal ~]$ ")
			if cmd == "":
				continue
			self._socket.sendall(cmd)
			output = ""
			recv = self._socket.recv(BUFF_SIZE)
			while True:
				if recv == END_FLAG:
					break
				if recv.endswith(END_FLAG):
					output += recv[:-len(END_FLAG)]
					break
				output += recv
				recv = self._socket.recv(BUFF_SIZE)
			if output[0] == "{":
				jset = json.loads(output)
				print jset["output"]
			else:
				print output
			if cmd == "exit":
				break
		self._socket.close()

