#!/bin/bash

curl -XPOST "http://elastic:4399elk@localhost:9200/frecom-filter-thread/test/_delete_by_query?pretty" -d '
{
	"query": {
		"match_all": {
		}
	}
}
'
