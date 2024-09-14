#!/usr/bin/bash

if [[ $# -lt 1 ]]; then 
    echo "usage: $0 <case_num>"
fi 

if [[ -d "input" ]]; then 
    rm -rf "input"
fi 

root_dir="/mnt/h/Study-L3T2/Assignments/CSE-314-Operating-Systems-Sessional/Assignment-1-Bash-Scripting/tests/given-test-cases/$1"

cp -R "${root_dir}/input" .

echo "copied testcase $1 successfully"

echo "running testcase $1"

./2005001.sh -i sample_input.txt