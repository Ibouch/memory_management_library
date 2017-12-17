#!/bin/sh

BASIC='\033[0;35m'
ME='\033[0;36m'
NC='\033[0m' # No Color

if [ -z "$1" ]; then
	echo "ERROR, please provide the test (choose between: '0', '1', '2', '3', '3_bis', '4')"
	exit
fi
path="test/test$1.c"
bin="test$1"
gcc -o $bin $path 

echo "${BASIC}$bin -- with 'real' malloc:${NC}"
/usr/bin/time -l "./$bin"

echo "${ME}$bin -- with my malloc:${NC}"
./run.sh /usr/bin/time -l "./$bin"
