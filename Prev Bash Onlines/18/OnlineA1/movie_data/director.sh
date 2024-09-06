#!/bin/bash

for file in *
do
    echo "$file"

    if [[ "$file" != *.txt ]]
    then
        continue
    fi

    movie=$(head -n 1 "$file")
    director=$(tail -n 2 "$file" | head -n 1)
    echo "$movie" 
    echo "$director"

    mkdir "$director" -p
    mv "$file" "$director"/
done