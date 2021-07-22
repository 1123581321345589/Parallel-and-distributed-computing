#!/bin/bash

GREEN='\033[1;32m'
NC='\033[0m'
YELLOW='\033[1;33m'
echo
echo -e ${GREEN}Compiling Test Suite...${NC}
temp=$(make clean)
make all
echo
echo -e ${GREEN}Submitting Test Suite to Cluster...${NC}
output=$(qsub pbs_A0)
echo $output
num="$(cut -d'.' -f1 <<<"$output")"
file=test_A0.o$num
while ! test -f "./$file"
do
    sleep 2
done
temp=$(make clean)
echo
echo -e ${GREEN}Open ${YELLOW}test_A0.o$num${GREEN} to see Test Results${NC}
echo
