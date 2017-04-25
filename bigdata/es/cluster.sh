#!/bin/bash

curl -XGET "http://elastic:4399elk@localhost:9200/_cluster/health?pretty"
