#!/usr/bin/env python
# encoding: utf-8

from lxml import etree


html = """<!doctype html>
<html class="theme-next use-motion theme-next-mist">
<head>
	<meta charset="UTF-8"/>
	<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
	<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1"/>
	<meta name="keywords" content="C,Python,Php,程序猿" />
	<link rel="alternate" href="/atom.xml" title="Astray Linux" type="application/atom+xml" />
	<title> Astray Linux </title>
</head>
</html>"""

#appoint which type of parser will use
parser = etree.HTMLParser()

doc = etree.fromstring(html, parser)

print "Title:", doc.xpath("/html/head/title/text()")[0]
print "Link href:", doc.xpath("//link/@href")[0]
print "meta list:", doc.xpath("//meta")
print "meta attr:", doc.xpath('//meta[@name="keywords"]/@content')[0]
print "meta attr:", doc.xpath("//meta[4]/@content")[0]
print "meta attr:", doc.xpath("//meta[1]/@charset")[0]

head = doc.xpath("/html/head")[0]

#node can execute xpath() too.
#'head' is a etree document too.
print "\nhead element:", head
print "head meta:", head.xpath("./meta[4]/@content")[0]
