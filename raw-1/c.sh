#!/usr/bin/bash

check_valid_dir(){
    if [ ! -d "$1" ]; then 
        echo "Error: '$1' is not a valid directory"
        exit 1
    fi
} 

check_valid_file(){
    if [ ! -f "$1" ]; then 
        echo "Error: '$1' is not a valid file"
        exit 1
    fi
} 

check_valid_file "a.sh"

echo "ok"
