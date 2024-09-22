# #!/opt/homebrew/bin/bash

# You are given a directory with many subdirectories. You need to copy all the files in another directory
# according to the length of their names (without extension).
# Command:
# ./online-1905XXX.sh input dir output dir
# Output:
# Files should be copied in output dir folder within a subdirectory of length size. 
# Inside each of these subdirectories, you should also rename files in ascending order of size 
# e. g. the file with the smallest size will have the name 0 its original name.txt 
# and the next one 1 its original name.txt and so on.
# txt is a placeholder extension. Use the original extension.

if [[ $# -ne 2 ]]; 
then
    echo "Invalid number of arguments"
    exit 1
fi
input_dir=$1
output_dir=$2
mkdir -p "$output_dir"
rm -rf $output_dir/*
> temp_file.txt
find "$input_dir" -type f | while read -r file_name;
do
    file_name_only=$(basename "$file_name" | cut -d '.' -f1)
    file_name_length=${#file_name_only}
    echo "$file_name_length" "$file_name" >> temp_file.txt
done
i=0
sort -n temp_file.txt | while read -r line; 
do
    name_len=$(echo $line | cut -d ' ' -f1)
    file_name=$(echo $line | cut -d ' ' -f2)
    file_name_only=$(basename "$file_name")
    new_file_name="${i}_${file_name_only}"
    mkdir -p "$output_dir/$name_len"
    cp "$file_name" "$output_dir/$name_len/$new_file_name"
    i=$((i+1))
done
rm temp_file.txt
