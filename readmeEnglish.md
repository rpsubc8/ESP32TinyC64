# Tiny ESP32 C64
Port of the Toy C64 emulator from PC x86 (Rink Springer) to the TTGO VGA32 v1.2 board with ESP32.
<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewC64.gif'></center>
I have made several modifications:
<ul>
 <li>Ported from x86 PC to ESP32</li>
 <li>Use of SRAM, no PSRAM is used.</li>
 <li>Runs on ESP32 520 KB SRAM (TTGO VGA32 v1.2)</li> 
 <li>Use of a single core</li> 
 <li>Created project compatible with Arduino IDE and Platform IO</li>
 <li>Floppy drive emulation (tests)</li>
 <li>Cartridge emulation (tests)</li> 
 <li>Monochrome and color video character mode emulation</li> 
 <li>RAM and speed optimization</li>
 <li>No Sound emulation (for now)</li>
 <li>No emulation of bitmap and sprite modes (for now).</li>
</ul>

<br>
<h1>Requirements</h1>
Required:
 <ul>
  <li>Visual Studio 1.48.1 PLATFORMIO 2.2.0</li>
  <li>Arduino IDE 1.8.11</li>  
  <li>Arduino bitluni 0.3.3 library (included in project)</li>
 </ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/ttgovga32v12.jpg'></center> 
<br> 

<h1>PlatformIO</h1>
PLATFORMIO 2.2.0 must be installed from the Visual Studio extensions.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewPlatformIOinstall.gif'></center>
Then select the working directory <b>TinyC64ttgovga32</b>.
We must modify the <b>platformio.ini</b> file <b>upload_port</b> option to select the COM port where we have our TTGO VGA32 board.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewPlatformIO.gif'></center>
Then we will proceed to compile and upload to the board. No partitions are used, so we must upload the whole compiled binary.
It's all set up so we don't have to install the bitluni libraries.

<br>
<h1>Arduino IDE</h1>
The whole project is compatible with the Arduino 1.8.11 framework.
We only have to open the <b>C64.ino</b> in the <b>c64</b> directory.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewArduinoIDEpreferences.gif'></center>
We must install the spressif extensions in the additional card url manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b>.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyC64/main/preview/previewArduinoIDElibrary.gif'></center>
For normal mode, the project is already prepared, so no bitluni library is needed. 
We must deactivate the PSRAM option, and in case of exceeding 1 MB of binary, select 4 MB of partition when uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and it will restart in loop mode.

<br>
<h1>Test</h1>
Since this is a Test version, it should only be used for performance testing.<br>
You get 162032 bytes of free SRAM.<br>
The emulation times are around 10 ms per frame and 49 fps.<br>
Only one core is being used for testing.
<pre>
fps:49 10645 m:10636 mx:10663
fps:49 10645 m:10634 mx:10662
fps:49 10638 m:10636 mx:10649
</pre>
Se puede probar el BASIC y pokes:
<pre>
poke 1024,1
poke 55296,1
</pre>
<br>

<br>
<h1>Usability</h1>
This version of Test only allows:
<ul>
 <li>Key <b>F12</b> Restart</li>
</ul>

<br>
<h1>Options</h1>
The <b>gbConfig.h</b> file options are selected:
<ul> 
 <li><b>usb_lib_fast_GetCyclesForOpcode</b>: Use array cache to calculate time per instruction (very fast).</li>
 <li><b>usb_lib_fast_MemoryReadByte</b>: Use vectorless shortcut when reading from memory (very fast)</li>
 <li><b>usb_lib_fast_MemoryWriteByte</b>: Use vectorless shortcut when writing from memory (very fast)</li>
</ul>

