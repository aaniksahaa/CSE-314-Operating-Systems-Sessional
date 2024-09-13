#!/usr/bin/bash

filename="example.7z"

# Extract the name (without extension) using `split`
IFS='.' read -r name extension <<< "$filename"

# Handle case where there is no extension
if [[ "$name" == "$extension" ]]; then
  extension=""
# else
#   name="${filename%.*}"
fi

echo "Name: $name"
echo "Extension: $extension"

# files=`ls ./assignment`

# for f in $files; do 
#     echo $f
# done 
