[![Build Status](https://travis-ci.org/FDSoftware/OpenEFI.svg?branch=master)](https://travis-ci.org/FDSoftware/OpenEFI)

<h1>Introducción:</h1>

<p>Open efi es un proyecto de EFI (electronic fuel inyection) que funciona con arduino y además es open source para que el que lo quiera utilizar lo pueda modificar a su gusto con conocimientos de programacion en C++)</p>
<img src="http://i.imgur.com/ucgET5K.jpg" width = 250;>

<p>Logo de Open-EFI :</p>
<img src="https://i.imgur.com/ISsDfFZ.png" width = 150;>
<h2>El proyecto:</h2>
El proyecto nació principalmente por 2 motivos: 
somos estudiantes de un colegio de automovilistica y surgió la idea de hacer arrancar un motor cuya ecu estaba faltante.
Solicitamos al instituto brindarnos una ecu programable y no hubo respuesta. Por este motivo decidimos hacerla nosotros mismos con ayuda de algunos profesores y decidimos hacerla libre para que cualquiera pueda usarla o incluso unirse al proyecto si consideramos valiosa su entrada

<h3>integrantes activos :</h3>
<p><a href="https://github.com/FDSoftware">FDSoftware</a></p>
<h3>ex integrantes:</h3>
<p><a href="https://github.com/SFADalmiro">SFADalmiro</a></p>
<p><a href="https://github.com/FrancisJPK">FrancisJPK</a></p>

<h4>Sobre el proyecto:</h4>
Hasta ahora tiene soporte para:
<ul>
	<li>Motores a nafta con inyeccion y encendido electrónico</li>
	<li>Soporte para GNC/GLP con tablas independientes</li>
	<li>Motores diesel con inyeccion directa</li>
	<li>Motores desde monocilindricos hasta de 8 cilindros</li>
</ul>

<h4>Funcionamiento:</h4>
<p>-Pseudo multithread para mayor eficiencia del programa</p>
<p>-Control de inyección en funcion de la carga, temperatura, velocidad de giro y presión de aire</p>
<p>-Control de chispa y avance con compensación por temperatura</p>
<p>-Parada de emergencia;</p>
<p>-Todos los datos importantes como los de la posición del volante de inercia, las rpm, la temperatura, las lecturas del TMAP, etc, están guardadas en variables al principio del código</p>
<p>-Todo el código con sus respectivos comentarios están en español</p>
<h4>Placas soportadas:</h4>
<ul>
	<li>Arduino DUE (sin PCB de referencia)</li>
	<li>Arduino MEGA(sin PCB de referencia)</li>
	<li>Arduino UNO (sin PCB de referencia)</li>
	<li>Arduino NANO (con PCB de referencia)</li>
	<li>ESP8266 (con PCB de referencia)</li>
</ul>

<h4>Extras:</h4>
<p> Programa de edicion, para reprogramar tablas de inyeccion, avance, ver codigos DTC y visualizacion de parametros:  <a href="https://github.com/FDSoftware/OpenEFI-Tuner">OpenEFI-Tuner</a>
</p>
