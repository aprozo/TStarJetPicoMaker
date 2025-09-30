#!/bin/bash
filelist="lists/all_pt_hat.list"   
#filelist="lists/test.list"
     
if [ ! -f "$filelist" ]; then
        echo "Error processing $filelist"
        exit 1
fi

./submit/submit.sh "$filelist"
    



