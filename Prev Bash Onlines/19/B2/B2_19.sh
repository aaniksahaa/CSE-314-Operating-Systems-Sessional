#!/opt/homebrew/bin/bash

# Copy those files that contain the name of any of the viruses listed in virus_list.txt. 
# While copying replace the virus name with 3 stars so virus names remain confidential.
# Command: 
# ./online-1905XXX.sh input dir virus list.txt output dir
# Output: 
# Files should be copied in output dir folder. 
# You should also rename files in ascending order of last time of modification in the file 
# e. g. the file with the latest modification time will have the name 0 its original name.txt
# and the next one 1 its original name.txt and so on.
# txt is a placeholder extension. Use the original extension

if [[ $# -ne 3 ]]; 
then
    echo "Invalid number of arguments"
    exit 1
fi
input_dir=$1
vir_file=$2
output_dir=$3
mkdir -p "$output_dir"
rm -rf "$output_dir"/*
> temp_file.txt
mapfile -t viruses < "$vir_file"
find "$input_dir" -type f | while read -r file_name;
do  
    for virus in "${viruses[@]}";
    do
        if grep -q "$virus" "$file_name";
        then
            file_basename=$(basename "$file_name")
            file_time=$(stat -f %m "$file_name")
            echo "$file_time" "$file_name" >> temp_file.txt
        fi
    done 
done
i=0
sort -n temp_file.txt | while read -r line;
do
    file_name=$(echo $line | cut -d ' ' -f2)
    file_basename=$(basename "$file_name")
    new_file_name="${i}_${file_basename}"
    cp "$file_name" "$output_dir/$new_file_name"
    for virus in "${viruses[@]}";
    do
        sed -i '' "s/$virus/***/g" "$output_dir/$new_file_name"
    done 
    i=$((i+1))
done
rm temp_file.txt

