#!/usr/bin/bash

if grep -q "d" a.txt; then
    echo "String found"
else
    echo "String not found"
fi
