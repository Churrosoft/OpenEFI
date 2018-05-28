<h1>Introducción:</h1>

<p >Open efi es un proyecto de EFI (electronic fuel inyection) que funciona con arduino y además es open source para que el que lo quiera utilizar lo pueda modificar a su gusto con conocimientos de programacion en arduino (c++)</p>
<img src="http://i.imgur.com/ucgET5K.jpg" width = 250;>

<p>Logo de Open-EFI :</p>
<img src="https://i.imgur.com/ISsDfFZ.png" width = 150;>
<h2>El proyecto:</h2>
El proyecto nació principalmente por 2 motivos: 
somos estudiantes de un colegio de automovilistica y surgió la idea de hacer arrancar un motor cuya ecu estaba faltante.
Solicitamos al instituto brindarnos una ecu programable y no hubo respuesta. Por este motivo decidimos hacerla nosotros mismos con ayuda de algunos profesores y decidimos hacerla libre para que cualquiera pueda usarla o incluso unirse al proyecto si consideramos valiosa su entrada

<h3>integrantes activos :</h3>
<p><a href="https://github.com/FDSoftware">FDSoftware</a>
Es el creador y coordinador del proyecto
Programador de C++, C#, y lenguajes web(php,javaScript)</p>
<h3>ex integrantes:</h3>
<p><a href="https://github.com/SFADalmiro">SFADalmiro</a>
Programador de c++ y c#. Con conocimientos de programación web. Armador y reparador de computadoras. Le gusta la programación de programas criptográficos;</p>
<p><a href="https://github.com/FrancisJPK">FrancisJPK</a>
Programador de c++ y java y pequeños conocimientos de programación web. Le gusta la programación de videojuegos;</p>

<h4>Sobre el proyecto:</h4>
El proyecto esta ahora en un 40% de completado, aunque probablemente una vez terminado siga creciendo y siendo optimizado y mejorado segun la aceptación que tenga en la comunidad.
Hasta ahora tiene soporte para:

<p>*Motores a nafta con inyeccion y encendido electrónico;</p>
<p>*Soporte para adaptación de inyectores de gas;</p>
<p>*motores diesel con inyectores por comando electronico;</p>
<p>*motores desde monocilindricos hasta de 8 cilindros;</p>

<h4>Funcionamiento:</h4>
<p>-Pseudo multithread para mayor eficiencia del programa</p>
<p>-Control de inyección en funcion de la carga, temperatura, velocidad de giro y presión de aire</p>
<p>-Control de chispa y avance de la misma en función de la temperatura y la velocidad de giro del motor</p>
<p>-Parada de emergencia;</p>
<p>-Todos los datos importantes como los de la posición del volante de inercia, las rpm, la temperatura, las lecturas del TMAP, etc, están guardadas en variables al principio del código</p>
<p>-Funciona (por ahora) solo con arduino DUE</p>
<p>-Todo el código con sus respectivos comentarios están en español</p>

<h4>Extras:</h4>
<p>
Recientemente <a href="https://github.com/FDSoftware">FDSoftware</a> hizo un programa que muestra de una forma visualmente atractiva y intuitiva, los datos en tiempo real de la open efi cuando está en funcionamiento. 
Se puede encontrar aquí: <a href="https://github.com/FDSoftware/OpenEFI-Tuner">Open-efi Tuner</a>
</p>
