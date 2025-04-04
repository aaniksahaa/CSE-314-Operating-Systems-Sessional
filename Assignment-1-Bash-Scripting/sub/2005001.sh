#!/usr/bin/bash

create_or_clear_directories(){
    for dir in $*; do
        if [[ -d "$dir" ]]; then
            rm -rf "$dir"/*
        else
            mkdir -p "$dir"
        fi
    done
}

show_usage(){
    echo "Invalid command line arguments"
    echo "Usage: <script_name.sh> -i <input_filepath>"
    exit 1
}

is_numeric(){
    if [[ $1 =~ ^[0-9]+$ ]]; then
        echo 1
    else 
        echo 0
    fi 
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
            if [[ $( is_numeric $v ) -ne 1 ]]; then 
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

########################################

root_dir=$( pwd )

input_dir="${root_dir}/input"
output_dir="${root_dir}/output"

create_or_clear_directories "$output_dir"

########################################

if [ $# -ne 2 ]; then
    show_usage
fi

input_file="${input_dir}/$2"

check_valid_file $input_file

# convert to unix format
# to get rid of CR/LF issues
dos2unix $input_file > /dev/null 2>&1

line_count=1

############ input file parsing ###################

while read line
do
    # trim the line from both ends 
    # removing leading and trailing spaces
    # line=$(echo "$line" | xargs)
    case $line_count in 
        1)  check_line "use_archived" "$line" 1 "true false"
            use_archived=$line
            ;;
        2)  if [[ "$use_archived" == "true" ]]; then 
                check_line "allowed_archive_formats" "$line" 0 "zip rar tar"
                read -a allowed_archive_formats <<< $line
            else 
                allowed_archive_formats=()
            fi 
            ;;
        3)  check_line "allowed_programming_languages" "$line" 0 "c cpp python sh"
            read -a allowed_programming_languages <<< $line
            has_python=0
            for lang in ${allowed_programming_languages[@]}; do 
                if [[ $lang == "python" ]]; then
                    has_python=1 
                fi
            done
            if [[ $has_python == 1 ]]; then
                allowed_programming_languages+=("py")
            fi
            ;;
        4)  check_line "total_marks" "$line" 1 "#"
            total_marks=$line
            ;;
        5)  check_line "penalty_unmatched" "$line" 1 "#"
            penalty_unmatched=$line
            ;;
        6)  working_dir="${input_dir}/$line"
            check_valid_dir $working_dir
            cp -R $working_dir $output_dir
            working_dir="${output_dir}/$line"
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
        8)  expected_output_file="${input_dir}/$line"
            check_valid_file $expected_output_file
            ;;
        9)  check_line "penalty_submission" "$line" 1 "#"
            penalty_submission=$line ;;
        10) plagiarism_analysis_file="${input_dir}/$line" 
            check_valid_file $plagiarism_analysis_file
            ;;
        11) check_line "plagiarism_penalty_pct" "$line" 1 "#"
            plagiarism_penalty_pct=$line 
            if [ $plagiarism_penalty_pct -lt 0 ] || [ $plagiarism_penalty_pct -gt 100 ]; then
                echo "Error: plagiarism_penalty_pct must be between 0 and 100"
                exit 1 
            fi 
            ;; 
    esac
    (( line_count++ ))
done < $input_file

if [ $line_count != 12 ]; then 
    echo "Error: Input is expected to have exactly 11 lines."
    exit 1 
fi

########## parsing end ############

##############################################

issues_dir="${working_dir}/issues"
checked_dir="${working_dir}/checked"

create_or_clear_directories "$issues_dir" "$checked_dir"

##############################################

temp_dir="$working_dir/temp"
mkdir -p $temp_dir

is_valid_id(){
    # expected args

    # $1 => id
    # $2 => min_id
    # $3 => max_id

    if [[ $(is_numeric $1) -ne 1 ]]; then 
        echo 0
    elif [[ $1 -lt $min_id || $1 -gt $max_id ]]; then 
        echo 0
    else 
        echo 1
    fi
}

is_archive_format(){
    archive_formats=("zip" "rar" "tar")
    for a in ${archive_formats[@]}; do 
        if [[ "$1" == "$a" ]]; then 
            echo 1
            return 0
        fi 
    done
    echo 0
}

is_valid_archive_format(){
    for a in ${allowed_archive_formats[@]}; do 
        if [[ "$1" == "$a" ]]; then 
            echo 1
            return 0
        fi 
    done
    echo 0
}

is_valid_language(){
    for a in ${allowed_programming_languages[@]}; do 
        if [[ "$1" == "$a" ]]; then 
            echo 1
            return 0
        fi 
    done
    echo 0
}

########## locate or initialize appropriate directories ###########

working_dir_listing=`ls $working_dir`

declare -A all_ids
declare -A folder_name
declare -A issue_case 

for f in $working_dir_listing; do 
    fullpath="$working_dir/$f"
    if [[ -d $fullpath ]]; then 
        if [[ "$(is_valid_id $f)" -eq 1 ]]; then
            all_ids["$f"]="true"
            folder_name["$f"]="$f"
            issue_case["$f"]=1
        elif [[ "$(is_numeric $f)" -eq 1 ]]; then
            all_ids["$f"]="true"
            issue_case["$f"]=5
            mv "$fullpath" "$issues_dir"
        fi 
    else 
        IFS='.' read -r name extension <<< "$f"
        if [[ "$name" == "$extension" ]]; then 
            extension=""
        fi 
        if [[ "$(is_numeric $name)" -eq 1 ]]; then 
            all_ids["$name"]="true"
            if [[ $(is_archive_format $extension) -eq 1 ]]; then
                rm -rf "$temp_dir"/*
                case $extension in 
                    zip)unzip -o $fullpath -d "$temp_dir" > /dev/null
                        ;;
                    rar)unrar -o+ x $fullpath "$temp_dir" > /dev/null
                        ;;
                    tar)tar --overwrite -xf $fullpath -C "$temp_dir" > /dev/null
                        ;;
                esac
                created_folder=$( ls $temp_dir | head -n1 )
                if [[ "$created_folder" != "$name" ]]; then 
                    issue_case["$name"]=4
                    mv "$temp_dir/$created_folder" "$temp_dir/$name"
                    created_folder="$name" 
                fi 
                if [[ "$(is_valid_id $name)" -eq 1 ]]; then 
                    if [[ $(is_valid_archive_format $extension) -ne 1 ]]; then
                        issue_case["$name"]=2
                        # mv "$temp_dir/$created_folder" "$issues_dir"
                    else 
                        folder_name["$name"]="$created_folder"
                        rm -rf "$working_dir/$created_folder"
                        mv "$temp_dir/$created_folder" "$working_dir"
                    fi
                else 
                    issue_case["$name"]=5
                    mv "$temp_dir/$created_folder" "$issues_dir"
                fi
                 
            else
                mkdir -p "$working_dir/$name"
                mv "$fullpath" "$working_dir/$name"
                if [[ $(is_valid_id $name) -eq 1 ]]; then
                    folder_name["$name"]="$name"
                else 
                    mv "$working_dir/$name" "$issues_dir"
                    issue_case["$name"]=5
                fi 
            fi 
        fi
    fi 
done

############### evaluate ###################

declare -A mismatch_penalty

for id in ${!folder_name[@]}; do
    fullpath="$working_dir/${folder_name[$id]}"
    files=$( find "$fullpath" -type f )
    found=0
    target_file=""
    for f in ${files[@]}; do
        basename=$( basename "$f" )
        IFS='.' read -r name extension <<< "$basename"
        if [[ "$name" == "$extension" ]]; then 
            extension=""
        fi
        if [[ "$name" == "$id" && $(is_valid_language $extension) -eq 1 ]]; then
            found=1
            target_file="$f"
            break
        fi
    done
    if [[ $found -ne 1 ]]; then
        issue_case["$id"]=3
    else 
        output_file="$fullpath/${id}_output.txt"
        case $extension in
            c)  gcc "$target_file" -o "$temp_dir/a.out"
                if [[ $? -eq 0 ]]; then
                    chmod +x "$temp_dir/a.out"
                    "$temp_dir/a.out" > "$output_file"
                else  
                    > "$output_file"
                fi
                ;;
            cpp)g++ "$target_file" -o "$temp_dir/a.out"
                if [[ $? -eq 0 ]]; then
                    chmod +x "$temp_dir/a.out"
                    "$temp_dir/a.out" > "$output_file"
                else 
                    > "$output_file"
                fi
                ;;
            py) python3 "$target_file" > "$output_file"
                ;;
            sh) chmod +x "$target_file" 
                "$target_file" > "$output_file"
                ;;
        esac 
        mismatch_penalty["$id"]=0
        while IFS= read line; do 
            if ! grep -Fxq "$line" "$output_file"; then
                mismatch_penalty["$id"]=$(( ${mismatch_penalty["$id"]} + $penalty_unmatched ))
            fi 
        done < "$expected_output_file"
    fi
done  

################ prepare penalty values ##################

declare -A submission_penalty

for id in ${!all_ids[@]}; do
    if [[ $(is_valid_id "$id") -ne 1 ]]; then
        submission_penalty["$id"]=$penalty_submission
    elif [[ -n "${folder_name[$id]}" ]]; then
        fullpath="$working_dir/${folder_name[$id]}"
        if [[ -n "${issue_case[$id]}" ]]; then
            if [[ ${issue_case[$id]} -eq 1 || ${issue_case[$id]} -eq 4 ]]; then 
                mv "$fullpath" "$checked_dir"
            else 
                mv "$fullpath" "$issues_dir"
            fi 
            submission_penalty["$id"]=$penalty_submission
        else
            mv "$fullpath" "$checked_dir"
            submission_penalty["$id"]=0
        fi
    else 
        if [[ -n "${issue_case[$id]}" ]]; then
            submission_penalty["$id"]=$penalty_submission
        fi
    fi
    if [[ -z ${mismatch_penalty[$id]} ]]; then 
        mismatch_penalty[$id]=0
    fi
    if [[ -z ${submission_penalty[$id]} ]]; then 
        submission_penalty[$id]=0
    fi 
done

all_ids_for_csv=()

for id in ${!all_ids[@]}; do 
    all_ids_for_csv+=("$id")
done 

for(( id=min_id; id<=max_id; id++ )); do 
    if [[ -z ${all_ids[$id]} ]]; then
        all_ids_for_csv+=("$id") 
    fi 
done 

sorted_ids=$( for i in ${all_ids_for_csv[@]}; do 
                    echo $i
                done | sort
                 )

marks_csv_file="${working_dir}/marks.csv"

> "$marks_csv_file"

echo "id,marks,marks_deducted,total_marks,remarks" > "$marks_csv_file"

for id in ${sorted_ids[@]}; do
    if [[ -z ${all_ids[$id]} ]]; then 
        row="${id},0,0,0,missing_submission " 
        echo $row >> "$marks_csv_file"
        continue
    fi 
    marks_deducted=$(( ${submission_penalty[$id]} ))
    if [[ -z ${folder_name[$id]} || ${issue_case[$id]} -eq 3 ]]; then
        marks=0 
    else 
        marks=$(( $total_marks - ${mismatch_penalty[$id]} ))
    fi  
    this_total_marks=$(( $marks - $marks_deducted ))
    remarks=""
    if [[ -n ${issue_case[$id]} ]]; then
        remarks+="issue case #${issue_case[$id]} "
    fi
    # handling plagiarism
    penalty_plagiarism=$(( ( $total_marks * $plagiarism_penalty_pct ) / 100 ))
    if grep -q "$id" "$plagiarism_analysis_file"; then
        this_total_marks=$(( - $penalty_plagiarism ))
        remarks+="plagiarism detected "
    fi
    row="${id},${marks},${marks_deducted},${this_total_marks},${remarks}" 
    echo $row >> "$marks_csv_file"
done 

echo "marks.csv written successfully"

rm -rf "${temp_dir}"