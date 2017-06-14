#!/bin/bash

rm -rf *.o eecs485pa6p
c++ -c -o eecs485pa6p.o eecs485pa6p.cpp
g++ eecs485pa6p.o -o eecs485pa6p -ldl -lpthread

./eecs485pa6p 0.85 -k 10 hadoop/dataset/mining.edges.xml proutput/pgrkoutput