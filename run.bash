#!/bin/bash

make
./program
rm t1_attr1_Sequential_indexFile.bin
rm t1_attr1_Sequential_auxFile.bin
rm t1_attr1_Sequential_duplicateFile.bin
make clean