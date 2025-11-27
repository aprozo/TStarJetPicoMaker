#!/bin/bash
filelist="lists/list_of_lists.list"   
# filelist="lists/diff.list" 
# filelist="lists/test.list"
     
if [ ! -f "$filelist" ]; then
        echo "Error processing $filelist"
        exit 1
fi

./submit/submit.sh "$filelist"
    



