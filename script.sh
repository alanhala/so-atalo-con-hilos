#!/bin/bash

git clone "https://github.com/sisoputnfrba/so-commons-library"
cd so-commons-library
sudo make install
sudo make all
cd src
sudo make install
sudo make all
cd
git clone "https://github.com/sisoputnfrba/ansisop-parser"
cd ansisop-parser
cd parser
sudo make install
sudo make all
cd
cd tp-2016-1c-Atalo-con-Hilos
cd Console
git clone "https://github.com/sisoputnfrba/scripts-ansisop"
cd scripts-ansisop
cd scripts
chmod +x *.ansisop
cd ../../
mv config_console.txt ./scripts-ansisop/scripts
cd
cd tp-2016-1c-Atalo-con-Hilos
cd SWAP
sudo make all
cd ../UMC
sudo make all
cd ../Kernel
sudo make all
cd ../CPU
sudo make all
cd ../Console
sudo make all
