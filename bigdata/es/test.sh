#!/bin/bash

#创建一个megacorp索引(数据库)，employee类型(表), 并新增一个文档(行)，编辑为1
curl -XPUT 'http://elastic:4399elk@localhost:9200/megacorp/employee/1?pretty' -d '
{
	"first_name": "john",
	"last_name": "Smith",
	"age": 25,
	"about": "I love to go rock climbing.",
	"interests": ["sports", "music"]
}'

#2号文档
curl -XPUT 'http://elastic:4399elk@localhost:9200/megacorp/employee/2?pretty' -d '
{
	"first_name": "Jane",
	"last_name": "Smith",
	"age": 32,
	"about": "I like to collect rock albums",
	"interests": ["music"]
}'

#3号文档
curl -XPUT 'http://elastic:4399elk@localhost:9200/megacorp/employee/3?pretty' -d '
{
	"first_name": "Douglas",
	"last_name": "Fir",
	"age": 35,
	"about": "I like to build cabinets",
	"interests": ["forestry"]
}'
