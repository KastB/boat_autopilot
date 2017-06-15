#!/bin/bash
if [  "$1" != '' ]
	then
	cp "$1" "$1"_processed.csv
	sed -i -- 's/,/./g' "$1"_processed.csv
	sed -i -- 's/\t/,/g' "$1"_processed.csv
else
	echo "please specify file to process"
fi
