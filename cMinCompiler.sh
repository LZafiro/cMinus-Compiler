#!/bin/bash

cd ./source
make
cd ..
./cmin entrada.txt
python3 ./source/assemblyAndBin-Generator.py

