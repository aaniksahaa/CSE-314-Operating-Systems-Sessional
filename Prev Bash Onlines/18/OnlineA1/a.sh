#!/usr/bin/bash

cnt=0

name=""
desc=""
dir=""
year=""

while read -r line; do
    if [[ ${#line} -le 1 ]]; then 
        cnt=$(( $cnt + 1 ))
    else 
        case $cnt in
            0) name+=$line;;
            1) desc+=$line;;
            2) dir+=$line;;
            3) year+=$line;;
        esac
    fi 
done < a.txt

echo $dir