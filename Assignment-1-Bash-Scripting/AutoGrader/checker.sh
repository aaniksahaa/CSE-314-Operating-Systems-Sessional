#!/usr/bin/bash

# if [[ $# -lt 1 ]]; then 
#     echo "usage: $0 <case_num>"
# fi 

declare -A verdicts

check(){
    if [[ -d "input" ]]; then 
        rm -rf "input"
    fi 

    root_dir="/mnt/h/Study-L3T2/Assignments/CSE-314-Operating-Systems-Sessional/Assignment-1-Bash-Scripting/tests/given-test-cases/$1"

    cp -R "${root_dir}/input" .

    echo "copied testcase $1 successfully"

    echo "running testcase $1"

    ./2005001.sh -i sample_input.txt

    marks_file="output/submissions/marks.csv"
    given_marks_file="$root_dir/output/submissions/marks.csv"

    diff_lines=$(diff $marks_file $given_marks_file | grep '^<' -c)

    echo ""
    echo "Number of mismatches = $diff_lines"

    if [[ $diff_lines -eq 0 ]]; then 
        verdicts[$i]="CORRECT"
    else 
        verdicts[$i]="NOT CORRECT"
    fi 
}

for i in {1..6}; do 
    check $i
    echo ""
done 

echo ""
for i in {1..6}; do 
    echo "Case #${i}: ${verdicts[${i}]}"
done 
