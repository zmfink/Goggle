#!/bin/bash

rm -rf *.o pagerank/eecs485pa6p
c++ -c -o pagerank/eecs485pa6p.o pagerank/eecs485pa6p.cpp
g++ pagerank/eecs485pa6p.o -o pagerank/eecs485pa6p -ldl -lpthread

./pagerank/eecs485pa6p 0.85 -k 50 hadoop/dataset/mining.edges.xml proutput/pgrkoutput