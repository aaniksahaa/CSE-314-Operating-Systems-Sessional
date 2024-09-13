#!/usr/bin/bash

folder=$(unzip -qql nf2.zip | head -n1 | awk '{print $4}' | sed 's@/.*@@')
echo "Extracted folder: $folder"
