#!/bin/bash

if [ $# -ne 1 ];then
    echo -e "input like: $0 <pid>\n"
    exit 1
fi

pig="$1"
#echo "pid=[${pig}]"

function F_printSubPid()
{
    #local tout=$(pgrep -w -P "$1")
    local tout=$(pgrep -P "$1")
    if [ -z "${tout}" ];then
        #echo "$1"
        return 0
    fi
    local tnaa
    echo "${tout}"|tac|while read tnaa
    do
        F_printSubPid "${tnaa}"
        echo "${tnaa}"

    done
    return 0
}

F_printSubPid "${pig}"
