cd /home/utnso;ls tp || mkdir tp 
cd tp

if ls /home/utnso/tp/tp-2016-1c-Atalo-con-Hilos; then
    echo 'YA ESTA BAJADO DEL REPOSITORIO NO BAJAR';
else
	git clone https://github.com/sisoputnfrba/tp-2016-1c-Atalo-con-Hilos.git;
fi
cd /home/utnso/tp/tp-2016-1c-Atalo-con-Hilos

cd Console
sudo make clean
sudo make all
cd ..
cd CPU
make clean
make all
cd ..
cd Kernel
make clean
make all
cp kernel_config2.txt kernel_config.txt
cd ..
cd SWAP
make clean
make all
cp config_file2.txt config_file.txt
cd ..
cd UMC
make clean
make all
cp umc2.cfg umc.cfg



if ls /home/utnso/tp/ansisop-parser; then
    exit 0
fi

cd /home/utnso/tp
if ls /home/utnso/tp/ansisop-parser; then
    echo 'YA ESTA BAJADO DEL REPOSITORIO NO BAJAR';
else
	git clone https://github.com/sisoputnfrba/ansisop-parser.git
	
fi

echo "DESCARGA DEL PARSER COMPLETA"
echo "PROCEDEMOS A INSTALARLA"
cd ansisop-parser/parser
sudo make all
sudo make install

cd /home/utnso/tp
if ls /home/utnso/tp/so-commons-library; then
    echo 'YA ESTA BAJADO DEL REPOSITORIO NO BAJAR';
else
	git clone https://github.com/sisoputnfrba/so-commons-library.git
	
fi
echo "DESCARGA DE LAS COMMONS COMPLETA"
echo "PROCEDEMOS A INSTALARLAS"
cd so-commons-library
sudo make all
sudo make install

