#!/usr/bin/bash

files=$( find . -type f )

for f in ${files[@]}; do 
    echo $( basename $f )
done 
