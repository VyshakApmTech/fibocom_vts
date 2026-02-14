#!/bin/bash
# Introduction:
#   Use this script to compress Fibocom_Linux_MakeDiff_Tool.7z
#
# NOTE:
#   Input Fibocom_Linux_MakeDiff_Tool_V1.1.0.XX for tool name.

ROOT_DIR=`pwd`

TOOL_NAME=$1
if [ "$TOOL_NAME" == "" ]; then
    echo -e "\033[32m=================================================== \033[0m"
    echo -e "\033[31;33mPlease input tools name. \nExample : Fibocom_Linux_MakeDiff_Tool_V1.1.0.13\033[0m"
    echo -en "\033[31;33mInput TOOL_NAME:\033[0m"; read TOOL_NAME
    echo -e "\033[32m=================================================== \033[0m"
fi
compress_tool_name=$TOOL_NAME.7z

cd merge_src/
make
cp merge $ROOT_DIR/merge
cd $ROOT_DIR

7z a $compress_tool_name bin/ doc/ merge ModifyRecord.txt

[ ! -f $compress_tool_name ] && echo -e "\033[31;33mError: Compress failed!\033[0m" && exit -1

echo -e "\033[32m===================================================================\033[0m"
echo -e "\033[31;33mCompress successful! File : $compress_tool_name\033[0m"
echo -e "\033[32m===================================================================\033[0m"
