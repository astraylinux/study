#!/usr/bin/env python
# encoding: utf-8
import urllib
import json
import fileinput
from ctypes import CDLL, c_void_p, c_char_p, c_uint, c_void_p, c_bool, c_int

libupdater = CDLL("./libupdate.so")

new_mongo_updater = libupdater.new_mongo_updater
new_mongo_updater.argtypes = []
new_mongo_updater.restype = c_void_p

destroy_mongo_updater = libupdater.destroy_mongo_updater
destroy_mongo_updater.argtypes = [c_void_p]
destroy_mongo_updater.restype = c_void_p

init_mongo = libupdater.init_mongo
init_mongo.argtypes = [c_void_p, c_char_p, c_char_p, c_char_p, c_uint]
init_mongo.restype = c_bool

update2mongo = libupdater.update2mongo
update2mongo.argtypes = [c_void_p, c_char_p]
update2mongo.restype = c_int

destroy_mongo = libupdater.destroy_mongo
destroy_mongo.argtypes = [c_void_p]
destroy_mongo.restype = c_void_p

#====================================

if __name__ == "__main__":
	rows = []
	LINE_LIMIT = 10
	count = 0
	for line in fileinput.input("./test.data"):
		clums = line.replace("\n", "").split("\t")
		item = {"_id":int(clums[0]), "name":clums[1]}
		rows.append(item)
		count += 1
		if count >= LINE_LIMIT:
			break


	jstr = json.dumps(rows)
	url = "mongodb://test:%s@localhost/?authSource=admin"%(\
			urllib.quote("$123"))
	updater = new_mongo_updater()
	if init_mongo(updater, url, "test", "test", 2):
		update2mongo(updater, jstr)
		destroy_mongo(updater)

	destroy_mongo_updater(updater)
	print "END."
