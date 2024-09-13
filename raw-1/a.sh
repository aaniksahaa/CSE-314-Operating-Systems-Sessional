#!/usr/bin/bash

show_usage(){
    echo "Invalid command line arguments"
    echo "Usage: <script_name.sh> -i <input_filepath>"
    exit 1
}

check_valid_dir(){
    if [ ! -d "$1" ]; then 
        echo "Error: '$1' is not a valid directory"
        exit 1
    fi
} 

check_valid_file(){
    if [ ! -f "$1" ]; then 
        echo "Error: '$1' is not a valid file"
        exit 1
    fi
} 

check_line(){
    # expected args

    # $1 => field name
    # $2 => line string
    # $3 => number of expected values 
    # $4 => allowed values (space-separated)
    
    # if $3 is 0    => any number of values is okay
    # if $4 is "*"  => then any string is allowed as values
    # if $4 is "#"  => then values must be numeric

    field_name=$1
    read -a values <<< $2
    num_expected_values=$3
    read -a allowed_values <<< $4
    
    num_values=${#values[@]}

    if [ $num_expected_values -ne 0 ]; then
        if [ "$num_values" -ne "$num_expected_values" ]; then 
            echo "Error: Expected $num_expected_values value(s) for $field_name, but found $num_values"
            exit 1
        fi
    fi

    # handling case of *, where any string is allowed
    if [ "$4" = "*" ]; then 
        return 0
    fi

    # handling case of #, where only numeric value is allowed
    if [ "$4" = "#" ]; then 
        for v in "${values[@]}"; do
            # checking if numeric
            if [[ ! $v =~ ^[0-9]+$ ]]; then 
                echo "Error: '$v' is not numeric. The value(s) in $field_name must be numeric."
                exit 1
            fi 
        done
        return 0 
    fi 

    # handling general case, matching against allowed values
    for v in "${values[@]}"; do
        matched=0
        for a in "${allowed_values[@]}"; do 
            if [ "$v" = "$a" ]; then 
                matched=1
                break
            fi 
        done 
        if [ $matched -eq 0 ]; then 
            echo "Error: '$v' is not a possible value in $field_name"
            echo "The only possible values in $field_name are: ${allowed_values[@]}"
            exit 1
        fi
    done 
}

if [ $# -ne 2 ]; then
    show_usage
fi

input_path=$2

dos2unix $input_path

line_count=1

while read line
do
    # trim the line from both ends 
    # removing leading and trailing spaces
    # line=$(echo "$line" | xargs)
    case $line_count in 
        1)  check_line "use_archived" "$line" 1 "true false"
            use_archived=$line
            ;;
        2)  check_line "allowed_archive_formats" "$line" 0 "zip rar tar"
            read -a allowed_archive_formats <<< $line
            ;;
        3)  check_line "allowed_programming_languages" "$line" 0 "c cpp python sh"
            read -a allowed_programming_languages <<< $line
            ;;
        4)  check_line "total_marks" "$line" 1 "#"
            total_marks=$line
            ;;
        5)  check_line "penalty_unmatched" "$line" 1 "#"
            penalty_unmatched=$line
            ;;
        6)  working_dir=$line
            check_valid_dir $working_dir
            ;;
        7)  read -a id_range <<< $line 
            check_line "id_range" "$line" 2 "#"
            num_ids=${#id_range[@]}
            min_id=${id_range[0]}
            max_id=${id_range[1]}
            if [ $max_id -lt $min_id ]; then 
                echo "Error: maximum ID is less than minimum ID"
                exit 1
            fi 
            ;;
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
