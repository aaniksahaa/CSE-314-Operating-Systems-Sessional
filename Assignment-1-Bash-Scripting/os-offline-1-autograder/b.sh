#!/usr/bin/bash

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

    for v in ${values[@]}; do
        matched=0
        for av in ${allowed_values[@]}; do 
            if [ "$v" = "$av" ]; then 
                matched=1
            fi 
        done 
        if [ $matched -eq 0 ]; then 
            echo "Error! '$v' is not allowed as $field_name"
            exit 1
        fi
    done 
}

check_line "use_archive" "a b" 2 "*"