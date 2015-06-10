#!/usr/bin/env python
# encoding: utf-8
""" TEST, argv to choose server or client"""
import sys
import server
import client

HOST = "127.0.0.1"
PORT = 9128

def server_test():
	cserver = server.CommandServer(HOST, PORT)
	cserver.run()

def client_test():
	cclent = client.CommandClient(HOST, PORT)
	cclent.run()

if __name__ == "__main__":
	if sys.argv[1] == "server":
		server_test()
	elif  sys.argv[1] == "client":
		client_test()
	else:
		print "wrong argument!"

