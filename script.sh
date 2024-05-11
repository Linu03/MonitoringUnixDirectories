#!/bin/bash

file_path="$1"

chmod 777 "$file_path"

rez="SAFE"

if grep -q -P '[^\x00-\x7F]' "$file_path"
then 
    rez=""
fi

if grep -q -E 'malware|dangerous|risk|attack' "$file_path"
then
    rez=""
fi

contor_linie=$(wc -l < "$file_path")
contor_cuvinte=$(wc -w < "$file_path")
contor_carac=$(wc -c < "$file_path")

if (( contor_linie < 3 && contor_cuvinte > 1000 && contor_carac > 2000 ))
then
    rez=""
fi

if [ -z "$rez" ]
then
    rez="$file_path"
fi

echo "$rez"
