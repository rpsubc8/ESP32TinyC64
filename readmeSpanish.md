
# Tiny ESP32 C64
Port del emulador Toy C64 de PC (Rink Springer) a la placa TTGO VGA32 v1.2 con ESP32.
<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewC64.gif'></center>
He realizado varias modificaciones:
<ul>
 <li>Portado de x86 PC a ESP32</li>
 <li>Uso de SRAM, no se usa PSRAM</li>
 <li>Funciona en ESP32 de 520 KB de RAM (TTGO VGA32)</li> 
 <li>Uso de un sólo core</li>
 <li>OSD de bajos recursos</li>
 <li>Creado proyecto compatible con Arduino IDE y Platform IO</li>
 <li>Emulación de disquetera</li>
 <li>Emulación de cartucho</li> 
 <li>Añadido modo de video en color</li>
 <li>Optimización de RAM y velocidad</li>
</ul> 
  
<br>
<h1>Requerimientos</h1>
Se requiere:
 <ul>
  <li>Visual Studio 1.48.1 PLATFORMIO 2.2.0</li>
  <li>Arduino IDE 1.8.11</li>  
  <li>Librería Arduino bitluni 0.3.3 (incluida en proyecto)</li>
 </ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/ttgovga32v12.jpg'></center> 
<br>
 
 
<h1>PlatformIO</h1>
Se debe instalar el PLATFORMIO 2.2.0 desde las extensiones del Visual Studio.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewPlatformIOinstall.gif'></center>
Luego se seleccionará el directorio de trabajo <b>TinyC64ttgovga32</b>.
Debemos modificar el fichero <b>platformio.ini</b> la opción <b>upload_port</b> para seleccionar el puerto COM donde tenemos nuestra placa TTGO VGA32.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewPlatformIO.gif'></center>
Luego procederemos a compilar y subir a la placa. No se usa particiones, así que debemos subir todo el binario compilado.
Está todo preparado para no tener que instalar las librerias de bitluni.


<br>
<h1>Arduino IDE</h1>
Todo el proyecto es compatible con la estructura de Arduino 1.8.11.
Tan sólo tenemos que abrir el <b>C64.ino</b> del directorio <b>CPCem</b>.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewArduinoIDEpreferences.gif'></center>
Debemos instalar las extensiones de spressif en el gestor de urls adicionales de tarjetas <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewArduinoIDElibrary.gif'></center>
Para el modo normal, ya está preparado el proyecto, de forma que no se necesita ninguna librería de bitluni. 
Debemos desactivar la opción de PSRAM, y en caso de superar 1 MB de binario, seleccionar 4 MB de partición a la hora de subir. Aunque el código no use PSRAM, si la opción está activa y nuestro ESP32 no dispone de ella, se generará una excepción y reinicio del mismo en modo bucle.



<br>
<h1>Usabilidad</h1>

 
<br>
<h1>Opciones</h1>
El archivo <b>gbConfig.h</b> se seleccionan las opciones: