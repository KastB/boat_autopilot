#!/bin/bash

#script to recursively travel a dir of n levels

function traverse() {   

for file in `ls $1`
do
    #current=${1}{$file}
    if [ ! -d ${1}/${file} ] ; then
		echo ${1}/${file}
		mv ${1}/${file} ${1}/${file}_tmp
        clang-format -style=Google ${1}/${file}_tmp > ${1}/${file}
        rm ${1}/${file}_tmp
    else
			echo "entering recursion with: ${1}${file}"
            traverse "${1}/${file}"
    fi
done
}

function main() {
    traverse $1
}

main $1
