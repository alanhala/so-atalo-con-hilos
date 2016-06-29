# tp-2016-1c-Atalo-con-Hilos
Primer Paso hay que copiar el codigo fuente nuestro en la VMServer para eso hay que hacer lo siguiente:

(2 alternativas o bajarlo de internet directamente o copiar por red llevando la notebook y conectandola a la red o poniendo un pendrive en otra compu con linux )

Copiar un directorio completo por red

scp -rpC [directorio] [ip]:[directorio]

Ejemplo: 

scp -rpC tp-2016-1c-Atalo-con-Hilos 192.168.3.129:/home/utnso

Descargar solo la última versión del código (en vez de todo el repositorio)

curl -u '[usuario]' -L -o [nombre_del_archivo_a_generar] [url_repo] 

Ejemplo: 

curl -u 'gastonprieto' -L -o commons_ultimo.tar https://api.github.com/repos/sisoputnfrba/so-commons-library/tarball/master

	*Este comando debe ejecutarse sin salto de línea. 

Luego descomprimir con: tar -xvf commons_ultimo.tar


en nuestro caso hay que bajar todo esto y instalar:


https://github.com/sisoputnfrba/tp-2016-1c-Atalo-con-Hilos.git

https://github.com/sisoputnfrba/scripts-ansisop.git


librerias de la catedra:

https://github.com/sisoputnfrba/so-commons-library.git

https://github.com/sisoputnfrba/ansisop-parser.git


 


Guía de Instalación de cada libreria (son 2)

    Instalar la biblioteca compartida commons (Guía de Instalación)
    cd parser: Ir al directorio de la biblioteca compartida
    make all: Compilar la biblioteca compartida (de no utilizar nuestro makefile, recordar linkear con la biblioteca de commons)
    ls build/: Revisar que exista el archivo libparser-ansisop.so dentro del directorio build/

    sudo make install: Instala la biblioteca, para que todos tengan acceso a la misma

    sudo make uninstall: Desinstala la biblioteca


Guía de Instalación de nuestro proyecto una vez ya copiado en la maquina.

    copiar proyecto a utnso
    cd : Ir al directorio de cada componente
    make all: crea un archivo executable en el mismo directorio llamado cpu,kernel, etc. , el ansisop lo copia directamente en el usr/bin para eso pide la clave utnso.

Para ejecutar las pruebas bajar los scripts-ansisop en la vm donde se instale la consola
https://github.com/sisoputnfrba/scripts-ansisop.git

asegurarse que cada script tenga modo de ejecucion  (por defecto ya vienen con modo de ejecucion por eso se puede saltear este paso)
chmod +x facil.ansisop
ejecutar cada script directamente poniendo
 ./facil.ansisop
