#!/usr/bin/env bash

add_license(){
    now=$(date)
    year=$(date +"%Y")
    contract_name=$1 
    file=$2
    echo "//" >> $file 
    echo "// $file " >> $file 
    echo "// $contract_name " >> $file 
    echo "//" >> $file 
    echo "// Created by $USER on $now" >> $file 
    echo "// Copyright © $year $USERNAME. All Rights Reserved" >> $file 
    echo "//" >> $file 
    echo >> $file
}

create_eos_cpp (){
    INCLUDE="#include <eosiolib/eosio.hpp>"
    cpp_file=$1.cpp
    hpp_file=$1.hpp 
    include_shield=$(echo "$1_HPP" | tr '[:lower:]' '[:upper:]')
    add_license $1 $cpp_file 
    add_license $1 $hpp_file
    echo $INCLUDE >> $cpp_file  
    echo $INCLUDE >> $hpp_file 
    echo >> $hpp_file
    echo "#ifndef $include_shield" >> $hpp_file
    echo "#define $include_shield" >> $hpp_file
    echo >> $hpp_file 
    echo "#endif // $include_shield" >> $hpp_file
}
    

if [ "$#" -eq 0 ]
then 
    echo "ERROR: Must specify directory name"
    exit 1
fi 

contract_name=$(basename $1)  

mkdir $1 
cd $1 
cp ../Makefile Makefile

rc_extension="_rc.md"
touch "$contract_name$rc_extension"

create_eos_cpp $contract_name 


