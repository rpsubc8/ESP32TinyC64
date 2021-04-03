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
