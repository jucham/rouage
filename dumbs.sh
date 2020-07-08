#!/bin/bash
for (( i=0; i<${1}; i++ ))
do
./Rouage dumb > dumb_$i.log &
echo "Dumb $i running"
done
