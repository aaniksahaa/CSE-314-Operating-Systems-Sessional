#!/usr/bin/bash

# Declare an unsorted array
my_array=(5 2 8 1 9 3)

# Sort the array and store the result in a new array
sorted_array=($(for i in "${my_array[@]}"; do echo "$i"; done | sort))

# Print the sorted array
echo "Sorted Array: ${sorted_array[@]}"

