#!/usr/bin/bash

input_path="sample_input.txt"

line_count=1

while read line
do
    # echo $line_count $line
    case $line_count in 
        1) use_archived=$line ;;
        2) read -a allowed_archive_formats <<< $line ;;
        3) read -a allowed_programming_languages <<< $line ;;
        4) total_marks=$line ;;
        5) penalty_unmatched=$line;;
        6) working_dir=$line;;
        7) read -a id_range <<< $line ; min_id=${id_range[0]}; max_id=${id_range[1]};;
        8) expected_output_path=$line ;;
        9) penalty_submission=$line ;;
        10) plagiarism_analysis_path=$line ;;
        11) plagiarism_penalty_pct=$line ;; 
    esac
    (( line_count++ ))
done < $input_path

echo $use_archived
echo ${allowed_archive_formats[0]}
echo $min_id
echo $max_id
