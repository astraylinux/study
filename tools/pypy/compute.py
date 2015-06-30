#!/usr/bin/env python
# encoding: utf-8
import sys
import hashlib
import time
import json
import hashlib

TEST_STR = "PyPy 的性能在没有 JIT 的情况下和 CPython 是差不多的 \
		(大概慢一到四倍 [1])，用了 JIT 就能超出几十到数百倍都有可能了，\
		这和其他的动态语言优化手段差不多，比如目前大部分的 JavaScript 引擎。\
		另外自动内存管理也是性能提升的一个因素，但相比 JIT 影响不大 [2]。\
		Unladen Swallow 的 JIT 是用 LLVM 实现的，最近这篇 US 开发者的总结\
		[3] 提到，他们一开始选择用 LLVM 是为了节省开发的工夫，但后来发现\
		LLVM 主要还是为了静态的语言生成实现的，用于 JIT 并不够给力。\
		相比起 PyPy 来 Unladen Swallow 更像一个改良的方案，所以优化可以发挥\
		的余地并不大。CPython 本身的 code base 质量是很高的，这也是为什么传\
		统优化方法不容易提高性能的原因。\
		另外，不要把 PyPy 简单理解为“自解释”，请至少读读 PyPy 网站的基本介绍。\
		[1] http://codespeak.net/pypy/dist/pypy/doc/faq.html#id19\
		[2] http://stackoverflow.com/questions/2591879/pypy-how-can-\
		it-possibly-beat-cpython\
		[3] http://qinsb.blogspot.com/2011/03/unladen-swallow-retrospective.html"


def base_str_operate():
	""" 基本字符串操作 """
	string = ""
	for index in range(0, 1000):
		string = string + TEST_STR
		string.replace("J", "").split(",")
		if not index%100:
			string = ""

def list_operate():
	""" 数组列表操作 """
	arr = []
	for index in range(0, 500000):
		arr.append(TEST_STR)
		if not index%1000:
			arr = arr[len(arr)/5:]

def dict_operate():
	""" 字典操作 """
	arr = {}
	for index in range(0, 50000):
		if not index in arr:
			arr[index] = TEST_STR
		if not index%300:
			arr.items()

def json_operate():
	""" json操作 """
	arr = {}
	for index in range(0, 100):
		arr[index] = TEST_STR
		jstr = json.dumps(arr, indent=4, ensure_ascii=False)
		json.loads(jstr)

def md5_operate():
	""" md5操作 """
	test_str = TEST_STR
	for index in range(0, 12):
		test_str += test_str + str(index)
	for index in range(0, 100):
		hashlib.md5(test_str).hexdigest()

def sort_operate():
	""" 排序操作"""
	test_str = TEST_STR
	for index in range(0, 10):
		test_str += test_str + str(index)
	for index in range(0, 300):
		sp_list = test_str.split("，")
		sp_list.sort()

def run(operate):
	start = time.time()
	operate()
	print "%-0.4f"%(time.time() - start)

if __name__ == "__main__":
	argv = sys.argv[1]
	if argv == "str":
		run(base_str_operate)
	if argv == "arr":
		run(list_operate)
	if argv == "dict":
		run(dict_operate)
	if argv == "json":
		run(json_operate)
	if argv == "md5":
		run(md5_operate)
	if argv == "sort":
		run(sort_operate)


