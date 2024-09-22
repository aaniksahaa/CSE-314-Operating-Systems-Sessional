#!/opt/homebrew/bin/bash

# Suppose your pc is under attack. All you know is that the virus is from input dir directory (which may
# contain sub-directories). Now you want to do a forensic analysis. For that, you need to copy all files with
# execute permission to a given output dir according to their user as a subdirectory. If there are multiple users,
# copy it to all. After copying, you also need to revoke their execution permission.
# Command:
# ./online-1905XXX.sh input dir output dir
# Output:
# Files should be copied in output dir folder within the appropriate subdirectory.
# if ls gives -rwxr–rwx 1 pikachu group1 0 Jan 19 12:59 choto.virus
# copy it to both output dir/pikachu and output dir/other
# You should also rename files in descending order of last time of modification in the file e. g. the file with
# the earliest modification time will have the name 0 its original name.txt and the next one 1 its original name.txt
# and so on.
# txt is a placeholder extension. Use the original extension.

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 input_dir output_dir"
    exit 1
fi
input_dir=$1
output_dir=$2
mkdir -p "$output_dir"
rm -r "$output_dir"/*
> temp_dir.txt
find "$input_dir" -type f | while read -r file; 
    do
    if [[ -x "$file" ]]; 
    then
        info=$(ls -l "$file")
        user=$(echo $info | cut -d ' ' -f3)
        group=$(echo $info | cut -d ' ' -f4)
        mod_time=$(stat -f %m "$file")
        echo $mod_time $file >> temp_dir.txt
    fi
done
i=0
sort -n temp_dir.txt | while read -r line;
do 
    file=$(echo $line | cut -d ' ' -f2)
    info=$(ls -l "$file")
    user=$(echo $info | cut -d ' ' -f3)
    group=$(echo $info | cut -d ' ' -f4)
    mkdir -p $output_dir/$user
    mkdir -p $output_dir/$group
    name=$(basename $file)
    file_name="${i}_${name}"
    cp $file $output_dir/$user/$file_name
    cp $file $output_dir/$group/$file_name
    i=$((i+1))
done
rm temp_dir.txt
