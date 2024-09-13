#!/usr/bin/bash

gcc a.c -o a.out
chmod +x a.out
./a.out > out.txt
rm -f a.out