#!/usr/bin/bash

show_usage(){
    echo "Invalid command line arguments"
    echo "Usage: <script_name.sh> -i <input_filepath>"
    exit 1
}

check_line(){
    # expected args

    # $1 => field name
    # $2 => line string
    # $3 => number of expected values 
    # $4 => allowed values (space-separated)
    
    # if $4 is "*" => then value is not checked

    field_name=$1
    read -a values <<< $2
    num_expected_values=$3
    read -a allowed_values <<< $4
    
    num_values=${#values[@]}

    if [ "$num_values" -ne "$num_expected_values" ]; then 
        echo "Expected $num_expected_values value(s) for $field_name, but found $num_values"
        exit 1
    fi


    if [ "$4" = "*" ]; then 
        return 0
    fi

    for v in "${values[@]}"; do
        matched=0
        for a in "${allowed_values[@]}"; do 
            if [ "$v" = "$a" ]; then 
                matched=1
                break
            fi 
        done 
        if [ $matched -eq 0 ]; then 
            echo "Error! $v is not allowed as $field_name"
            exit 1
        fi
    done 
}

if [ $# -ne 2 ]; then
    show_usage
fi

input_path=$2

line_count=1

while read line
do
    # trim the line from both ends 
    # removing leading and trailing spaces
    # line=$(echo "$line" | xargs)
    case $line_count in 
        1)  use_archived=$line
            check_line "use_archived" "$line" 1 "true false"
            ;;
        2)  read -a allowed_archive_formats <<< $line ;;
        3)  read -a allowed_programming_languages <<< $line ;;
        4)  total_marks=$line ;;
        5)  penalty_unmatched=$line;;
        6)  working_dir=$line;;
        7)  read -a id_range <<< $line 
            num_ids=${#id_range[@]}
            if [ $num_ids -ne 2 ]; then 
                echo "Error! ID Range must contain exactly 2 values, found $num_ids"
                exit 1
            fi
            min_id=${id_range[0]}
            max_id=${id_range[1]};;
        8)  expected_output_path=$line ;;
        9)  penalty_submission=$line ;;
        10) plagiarism_analysis_path=$line ;;
        11) plagiarism_penalty_pct=$line ;; 
    esac
    (( line_count++ ))
done < $input_path

echo $use_archived
echo ${allowed_archive_formats[0]}
echo $min_id
echo $max_id
