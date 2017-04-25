#!/bin/bash

#curl -XGET "http://elastic:4399elk@localhost:9200/megacorp/employee/2?pretty"
#curl -XGET "http://elastic:4399elk@localhost:9200/megacorp/employee/_search?pretty"
#curl -XGET "http://elastic:4399elk@localhost:9200/megacorp/employee/_search?pretty&q=last_name:smith"


#curl -XGET "http://elastic:4399elk@localhost:9200/megacorp/employee/_search?pretty" -d '
#{
#	"query": {
#		"match": {
#			"last_name": "smith"
#		}
#	}
#}
#'

#参考https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-bool-query.html
#curl -XGET "http://elastic:4399elk@localhost:9200/megacorp/employee/_search?pretty" -d '
#{
#	"query": {
#		"bool": {
#			"filter": {
#				"range": {"age": {"gt": 20}}
#			},
#			"must":{
#				"term": {"last_name": "smith"}
#			}
#		}
#	}
#}
#'

curl -XGET "http://elastic:4399elk@localhost:9200/megacorp/employee/_search?pretty" -d '
{
	"query": {
		"bool": {
			"filter": {
				"range": {"age": {"gt": 20}}
			},
			"must":{
				"match": {"about": "rock climbing"}
			}
		}
	},
	"highlight": {
		"fields": {
			"about": {}
		}
	}
}
'
