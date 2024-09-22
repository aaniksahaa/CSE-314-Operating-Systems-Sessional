# #!/opt/homebrew/bin/bash

# You are given a directory with many sub-directories, where there are many files. 
# Some of them are very confidential. So the owner has renamed the files with random extensions. 
# You have to copy the text files (ASCII format) to a particular directory in your secured Pendrive. 
# Do it fast, or else the owner will know about your deeds, and both your life and our country will be in danger.
# Command:
# ./online-1905XXX.sh input dir output dir
# Output:
# Files should be copied in output dir folder. You should also rename files in ascending order of the number of
# lines in the file e. g. the file with the smallest line count will have name 0 its original name.txt and the next
# one 1 its original name.txt and so on.
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
for file_name in "$input_dir"/*; 
do
    text=$(file "$file_name")
    if file "$file_name" | grep -q "ASCII text"; 
    then
        line_count=$(wc -l < $file_name)
        echo $line_count $(basename $file_name) >> temp_file.txt
    fi
done
i=0
sort -n temp_file.txt | while read -r line; 
do
    line_count=$(echo $line | cut -d ' ' -f1)
    file_name=$(echo $line | cut -d ' ' -f2)
    new_file_name="${i}_${file_name}"
    cp "$input_dir/$file_name" "$output_dir/$new_file_name"
    i=$((i+1))
done
rm temp_file.txt
