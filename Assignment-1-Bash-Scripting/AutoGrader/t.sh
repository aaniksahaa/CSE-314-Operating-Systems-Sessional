#!/usr/bin/bash

declare -A map 

map["a"]=1
map["b"]=2

if [[ map["c"] != 2 ]]; then 
    echo "ok"
fi 