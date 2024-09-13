#!/usr/bin/bash

folder=$(unzip -qql file.zip | head -n1 | awk '{print $4}' | sed 's@/.*@@')
unzip -o file.zip
echo "Extracted folder: $folder"
