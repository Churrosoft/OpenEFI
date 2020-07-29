[![Gitpod ready-to-code](https://img.shields.io/badge/Gitpod-ready--to--code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/openefi/OpenEFI)

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com) [![Build Status](https://travis-ci.org/openefi/OpenEFI.svg?branch=master)](https://travis-ci.org/openefi/OpenEFI)
# OpenEFI: 

OpenEFI es un sistema de inyección OpenSource configurable y adaptable a casi cualquier vehiculo con motores a Nafta/Gasohol/GNC/GLP

Está planeado el soporte a corto y largo plazo para:
-   Motores a nafta con inyección y encendido electrónico
-  calculo de tiempo de inyección a partir de tablas pre-definidas | Alpha Plus | Speed Density
-  Soporte de inyectores de alta impedancia (los de baja más adelante por requerir PWM además del tiempo de inyeccion)
-   Soporte para GNC/GLP con tablas independientes
-   Motores diesel con inyección directa
-   Motores de 1 a 8 cilindros

## Sobre el Software
la plataforma se ejecuta desde un STM32F103, conocido como "bluepill", para facilitar la programación del mismo, se utiliza el bootloader: [dapboot](https://github.com/devanlai/dapboot)


### Como generar los binarios:
se necesitan las siguientes herramientas:

    make
    arm-none-eabi-gcc 
    arm-none-eabi-newlib

pasos para grabar el firmware:

 1. Compile y grabe el bootloader => [dapboot](https://github.com/devanlai/dapboot)
 2. clone el repositorio con git o descarguelo
 3. dentro del repo ejecutar: `make flash-usb`

## Sobre el Hardware

### Preguntas frecuentes:
### ¿hay algún video sobre el funcionamiento de la ECU / EFI ?
No, por el momento solo tenemos avances en el diseño de los pcb's, pero todavía el hardware ni el software se encuentra preparado para una prueba real en un motor, estimamos nuestro primer test a finales del 2020 si es que no nos morimos antes
### ¿cómo hacen para adaptar el hardware a una configuración tan amplia?
Porque el tratamiento de las señales analógicas para la mayoría de los sensores son iguales, consta de un filtro paso bajo y un diodo para evitar el sobre voltaje, se intentan mantener simples para que sea fácil de mantener, luego se aplican otros filtros en software para mejorar aún más esto

### ¿se pueden revisar los esquemas en algún lado, y obtener más información en general de la placa?
Claro!, todo lo referido al hardware se encuentra en [OpenEFI-PCB](https://github.com/openefi/OpenEFI-PCB)

## Sobre la configuracion

### Preguntas frecuentes:
### ¿como se va a configurar todo esto?
para la configuracion inicial, y para la visualizacion de parametros en tiempo real, se utiliza [OpenEFI-Tuner](https://github.com/openefi/OpenEFI-Tuner) , la idea es que tengas unicamente que conectar la EFI a tu computadora, entrar a [tuner.openefi.xyz](http://tuner.openefi.xyz/) y empezar a jugar, sin tener que pasar por la tortura de instalar drivers, programas,etc , aunque tambien puedes montar tu propio OpenEFI-Tuner en tu servidor local si quieres !, aunque la aplicación web funcionara sin conexión a internet

### ¿tiene soporte para un scanner común?¿cómo se leen los DTC's?
No, por el momento no tenemos planeado agregar soporte a CAN hasta llegar al MVP, así que no se pueden usar scanners automotrices normales acá
para leer los DTC se utiliza OpenEFI-Tuner

[![Invitame un café en cafecito.app](https://cdn.cafecito.app/imgs/buttons/button_5.svg)](https://cafecito.app/openefi)