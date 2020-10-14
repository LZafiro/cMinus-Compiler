#!/bin/bash

cd ./source
make
cd ..
./cmin entrada.cmin
python3 ./source/assemblyAndBin-Generator.py

