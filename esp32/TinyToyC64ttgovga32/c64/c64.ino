//Port Toy C64 (Rink Springer) emulator to TTGO VGA32 by ackerman:
// Load Roms
// MODE400x300
// Mode 64 colors RGB 6 bits
// VGA library bitluni 0.3.3 (include)
// gbConfig options configuration compile

#include "Emulator/Keyboard/PS2Kbd.h"
#include <Arduino.h>
#include "gbConfig.h"
#include "c64.h"
#include "MartianVGA.h"
#include "def/Font.h"
#include "def/hardware.h"
#include "driver/timer.h"
#include "soc/timer_group_struct.h"
#include "gb_globals.h"
#include "dataFlash/gbrom.h"
#include "PS2Boot/PS2KeyCode.h"
#include "cia1.h"
#include "cia2.h"
#include "cpu6502.h"
#include "iecbus.h"
#include "iec1541.h"
#include "memory.h"
#include "vic.h"
#include "hostio.h"

VGA6Bit vga;

unsigned char gb_run_emulacion = 1; //Ejecuta la emulacion
unsigned char gb_current_ms_poll_sound = gb_ms_sound;
unsigned char gb_screen_xOffset=0;
static unsigned long gb_time_ini_espera;
static unsigned long gb_currentTime;
static unsigned long gb_sdl_time_sound_before;
static unsigned long gb_keyboardTime;
static unsigned long gb_time_ini_beep;
unsigned char gb_current_sel_rom=0;

unsigned char gb_show_osd_main_menu=0;

unsigned char gb_current_delay_emulate_ms= gb_delay_emulate_ms;
unsigned char gb_current_ms_poll_keyboard = gb_ms_keyboard;
unsigned char gb_delay_t=0;
unsigned char gb_delay_sound=0;

static unsigned char gbVolMixer=0; //Beep
static unsigned char gbCont=0;


static unsigned int gb_stats_time_cur= 0;
static unsigned int gb_stats_time_min= 500000;
static unsigned int gb_stats_time_max= 0;

//Datos
uint8_t * gb_vic_regs= NULL;
uint8_t * gb_memory= NULL;
Memory oMemory;
CPU6502 oCPU(oMemory);
HostIO oHostIO;
IECBus oIECBus;
IEC1541 oFDD;
VIC * oVIC;
CIA1 * oCIA1;
CIA2 * oCIA2;
VIC * gb_ptr_oVIC;
CIA1 * gb_ptr_oCIA1;
CIA2 * gp_ptr_oCIA2;
//int frame = 0;
unsigned char **gb_buffer_vga;
static unsigned long gb_vgaTime;

//Funciones
void Setup(void);
void SDL_keys_poll(void);
void do_tinyOSD(void);




//Very small tiny osd
void do_tinyOSD() 
{
}

//Lectura teclado
void SDL_keys_poll()
{  
  //oCIA1->TestTeclado(59,(keymap[PS2_KC_2])); 
  //row 0
  oCIA1->TestTeclado(0,(keymap[KEY_BACKSPACE]));
  oCIA1->TestTeclado(1,(keymap[PS2_KC_ENTER]));
	oCIA1->TestTeclado(2,(keymap[KEY_CURSOR_RIGHT]));
  oCIA1->TestTeclado(3,(keymap[PS2_KC_F7]));
  oCIA1->TestTeclado(4,(keymap[PS2_KC_F1]));
  oCIA1->TestTeclado(5,(keymap[PS2_KC_F3]));
  oCIA1->TestTeclado(6,(keymap[PS2_KC_F5]));
  oCIA1->TestTeclado(7,(keymap[KEY_CURSOR_DOWN]));	
	// row 1
  oCIA1->TestTeclado(8,(keymap[PS2_KC_3]));
  oCIA1->TestTeclado(9,(keymap[PS2_KC_W]));
  oCIA1->TestTeclado(10,(keymap[PS2_KC_A]));
  oCIA1->TestTeclado(11,(keymap[PS2_KC_4]));
  oCIA1->TestTeclado(12,(keymap[PS2_KC_Z]));
  oCIA1->TestTeclado(13,(keymap[PS2_KC_S]));
  oCIA1->TestTeclado(14,(keymap[PS2_KC_E]));
  oCIA1->TestTeclado(15,(keymap[PS2_KC_L_SHIFT]));
	// row 2
  oCIA1->TestTeclado(16,(keymap[PS2_KC_5]));
  oCIA1->TestTeclado(17,(keymap[PS2_KC_R]));
  oCIA1->TestTeclado(18,(keymap[PS2_KC_D]));
  oCIA1->TestTeclado(19,(keymap[PS2_KC_6]));
  oCIA1->TestTeclado(20,(keymap[PS2_KC_C]));
  oCIA1->TestTeclado(21,(keymap[PS2_KC_F]));
  oCIA1->TestTeclado(22,(keymap[PS2_KC_T]));
  oCIA1->TestTeclado(23,(keymap[PS2_KC_X]));	
	// row 3
  oCIA1->TestTeclado(24,(keymap[PS2_KC_7]));
  oCIA1->TestTeclado(25,(keymap[PS2_KC_Y]));
  oCIA1->TestTeclado(26,(keymap[PS2_KC_G]));
  oCIA1->TestTeclado(27,(keymap[PS2_KC_8]));
  oCIA1->TestTeclado(28,(keymap[PS2_KC_B]));
  oCIA1->TestTeclado(29,(keymap[PS2_KC_H]));
  oCIA1->TestTeclado(30,(keymap[PS2_KC_U]));
  oCIA1->TestTeclado(31,(keymap[PS2_KC_V]));	
	// row 4
  oCIA1->TestTeclado(32,(keymap[PS2_KC_9]));
  oCIA1->TestTeclado(33,(keymap[PS2_KC_I]));
  oCIA1->TestTeclado(34,(keymap[PS2_KC_J]));
  oCIA1->TestTeclado(35,(keymap[PS2_KC_0]));
  oCIA1->TestTeclado(36,(keymap[PS2_KC_M]));
  oCIA1->TestTeclado(37,(keymap[PS2_KC_K]));
  oCIA1->TestTeclado(38,(keymap[PS2_KC_O]));
  oCIA1->TestTeclado(39,(keymap[PS2_KC_N]));
	// row 5
  oCIA1->TestTeclado(40,(keymap[PS2_KC_KP_PLUS]));
  oCIA1->TestTeclado(41,(keymap[PS2_KC_P]));
  oCIA1->TestTeclado(42,(keymap[PS2_KC_L]));
	oCIA1->TestTeclado(43,(keymap[PS2_KC_MINUS]));
  oCIA1->TestTeclado(44,(keymap[PS2_KC_DOT]));
  //m_kbdmap[SDLK_COLON] = 45;
  oCIA1->TestTeclado(45,(keymap[PS2_KC_F8]));//F8 :
  oCIA1->TestTeclado(46,(keymap[PS2_KC_F9]));//F9 @
  oCIA1->TestTeclado(47,(keymap[PS2_KC_COMMA]));	
	// row 6
	oCIA1->TestTeclado(49,(keymap[PS2_KC_OPEN_SQ])); //SDLK_LEFTBRACKET asterisk
  //oCIA1->TestTeclado(50,(keymap[])); //SDLK_SEMICOLON
  //oCIA1->TestTeclado(51,(keymap[])); //SDLK_HOME
  oCIA1->TestTeclado(52,(keymap[PS2_KC_R_SHIFT])); //SDLK_RSHIFT    
  oCIA1->TestTeclado(53,(keymap[PS2_KC_EQUAL]));
  oCIA1->TestTeclado(54,(keymap[KEY_CURSOR_UP]));  
  //oCIA1->TestTeclado(55,(keymap[]));//SDLK_SLASH  		
	// row 7
  oCIA1->TestTeclado(56,(keymap[PS2_KC_1]));
  oCIA1->TestTeclado(57,(keymap[KEY_CURSOR_LEFT]));
  oCIA1->TestTeclado(58,(keymap[PS2_KC_CTRL])); //SDLK_LCTRL
  oCIA1->TestTeclado(59,(keymap[PS2_KC_2]));
  oCIA1->TestTeclado(60,(keymap[PS2_KC_SPACE]));
  //oCIA1->TestTeclado(61,(keymap[])); //SDLK_LALT commodore
  oCIA1->TestTeclado(62,(keymap[PS2_KC_Q]));
  oCIA1->TestTeclado(63,(keymap[PS2_KC_TAB]));	// run/stop				

  if (keymap[PS2_KC_F12] == 0)
  {
   ESP.restart();
  }

  /*key[0x02] = (keymap[PS2_KC_2] == 0)?1:0; //2
  key[0x01] = (keymap[PS2_KC_1] == 0)?1:0; //1
  key[0x03] = (keymap[PS2_KC_3] == 0)?1:0; //3
  key[0x0C] = (keymap[PS2_KC_4] == 0)?1:0; //4
  key[0x04] = (keymap[PS2_KC_Q] == 0)?1:0; //q
  key[0x05] = (keymap[PS2_KC_W] == 0)?1:0; //w
  key[0x06] = (keymap[PS2_KC_E] == 0)?1:0; //e  
  key[0x0D] = (keymap[PS2_KC_R] == 0)?1:0; //r
  key[0x07] = (keymap[PS2_KC_A] == 0)?1:0; //a
  key[0x08] = (keymap[PS2_KC_S] == 0)?1:0; //s
  key[0x09] = (keymap[PS2_KC_D] == 0)?1:0; //d
  key[0x0E] = (keymap[PS2_KC_F] == 0)?1:0; //f
  key[0x0A] = (keymap[PS2_KC_Z] == 0)?1:0; //z
  key[0x00] = (keymap[PS2_KC_X] == 0)?1:0; //x
  key[0x0B] = (keymap[PS2_KC_C] == 0)?1:0; //c
  key[0x0F] = (keymap[PS2_KC_V] == 0)?1:0; //v  

  if (keymap[KEY_CURSOR_LEFT] == 0)
   key[0x04] = 1;
  if (keymap[KEY_CURSOR_UP] == 0)
   key[0x05] = 1;
  if (keymap[KEY_CURSOR_RIGHT] == 0)
   key[0x06] = 1;
  if (keymap[KEY_CURSOR_DOWN] == 0)
   key[0x08] = 1; 
  if (keymap[KEY_BACKSPACE] == 0)
   key[0x0F] = 1;
   */
}







//Setup principal
void setup()
{ 
 #ifdef use_lib_log_serial
  Serial.begin(115200);         
  Serial.printf("HEAP BEGIN %d\n", ESP.getFreeHeap()); 
 #endif
 
 gb_vic_regs = (uint8_t*) malloc(1024); 
 gb_memory = (uint8_t*) malloc(65536);
 oMemory.AssignPtrMemory(gb_memory);    
 oCPU.AssignPtrMemory6502(gb_memory);

 oVIC = new VIC(oCPU, oMemory, oHostIO);
 oVIC->AssignPtrRegs(gb_vic_regs);	
 oCIA1 = new CIA1(oCPU, oMemory, oHostIO);
 oCIA2= new CIA2(oCPU, oMemory, *oVIC, oIECBus);
 oCIA1->Reset(); //Resetea teclado

 oMemory.ReadFromFlash(gb_rom_basic_901226_01,Memory::s_basic_base, Memory::s_basic_length);
 oMemory.ReadFromFlash(gb_rom_char_901225_01, Memory::s_charrom_base, Memory::s_charrom_length);
 oMemory.ReadFromFlash(gb_rom_kernel_901227_03, Memory::s_kernal_base, Memory::s_kernal_length); 

 if (!oHostIO.Initialize())
 {
	Serial.printf("can't initialize host I/O");
 }

 gb_ptr_oVIC = oVIC;
 gb_ptr_oCIA1 = oCIA1;
 gp_ptr_oCIA2 = oCIA2;
 oMemory.RegisterPeripheral(&(*oVIC), Memory::s_vic_base, Memory::s_vic_length);
 oMemory.RegisterPeripheral(&(*oCIA1), Memory::s_cia1_base, Memory::s_cia1_length);
 oMemory.RegisterPeripheral(&(*oCIA2), Memory::s_cia2_base, Memory::s_cia2_length);
 oCPU.Reset();
 

 oIECBus.RegisterDevice(8, &oFDD);

 oCPU.Dump();

 #define RED_PINS_6B 21, 22
 #define GRE_PINS_6B 18, 19
 #define BLU_PINS_6B  4,  5
 const int redPins[] = {RED_PINS_6B};
 const int grePins[] = {GRE_PINS_6B};
 const int bluPins[] = {BLU_PINS_6B};
 vga.init(vga.MODE400x300.custom(384,264), redPins, grePins, bluPins, HSYNC_PIN, VSYNC_PIN);
 
 //vga.setFont(Font6x8);
 vga.clear(BLACK);
 vga.fillRect(0,0,400,300,0x00);
 gb_buffer_vga= vga.backBuffer;
 PreparaColorVGA();

 //#ifdef use_lib_200x150
 // vga.fillRect(0,0,200,150,BLACK);
 // vga.fillRect(0,0,200,150,BLACK);//fix mode fast video
 //#else
 // vga.fillRect(0,0,320,200,BLACK);
 //vga.fillRect(0,0,320,200,BLACK);//fix mode fast video
 //#endif
 
 #ifdef use_lib_log_serial
  Serial.printf("VGA %d\n", ESP.getFreeHeap()); 
 #endif

 kb_begin();


 gb_keyboardTime = gb_vgaTime= gb_currentTime = gb_sdl_time_sound_before= gb_time_ini_beep = millis();

 #ifdef use_lib_log_serial  
  Serial.printf("END SETUP %d\n", ESP.getFreeHeap()); 
 #endif 
}



static unsigned int gb_jj_cur_cycle=0;
static unsigned int gb_jj_ini_cycle=0;
static unsigned int gb_jj_cur_fps=0;
static unsigned int gb_jj_ini_fps=0;
static unsigned int gb_jj_time_ini_fps=0;

//Bucle dibujar un frame
void jj_loop_cpu_one_frame()
{
 unsigned int ini_time= micros();
 while ((gb_jj_cur_cycle - gb_jj_ini_cycle) < 19656)
 {//one frame execute
  //oCIA1->Update();
  oCIA1->UpdateCIA();
  oIECBus.Update();
  gb_jj_cur_cycle+= oCPU.RunOpcode();
 } 
 gb_jj_cur_fps++;
 gb_jj_ini_cycle = gb_jj_cur_cycle;

 ini_time= micros() - ini_time;
 gb_stats_time_cur= ini_time;
 if (gb_stats_time_cur < gb_stats_time_min)
  gb_stats_time_min= gb_stats_time_cur;
 if (gb_stats_time_cur > gb_stats_time_max)
  gb_stats_time_max= gb_stats_time_cur; 


 //printf("%d\n",gb_jj_cur_cycle);
 //fflush(stdout);
}


//Loop main
void loop() 
{ 
 gb_currentTime = millis();
 if ((gb_currentTime-gb_keyboardTime) >= gb_current_ms_poll_keyboard)
 {  
  gb_keyboardTime = gb_currentTime;
  SDL_keys_poll();
 }

 //oCIA1->Update();
 //oIECBus.Update();


 //Completar 19656 ciclos 1 frame
 jj_loop_cpu_one_frame();

  //gb_currentTime = millis();
 gb_currentTime = millis();
 if ((gb_currentTime-gb_vgaTime) >= 20)
 {//50 fps
  gb_vgaTime= gb_currentTime;
  jj_video_Update();
  //oVIC->Update(); 
 }

 gb_currentTime = millis();
 if ((gb_currentTime-gb_jj_time_ini_fps)>=1000)
 {
  gb_jj_time_ini_fps=gb_currentTime;
  unsigned int aux_fps= gb_jj_cur_fps-gb_jj_ini_fps;
  gb_jj_ini_fps = gb_jj_cur_fps;  
  Serial.printf ("fps:%d %d m:%d mx:%d\n",aux_fps,gb_stats_time_cur,gb_stats_time_min,gb_stats_time_max);
  gb_stats_time_min= 500000;
  gb_stats_time_max= 0;  
 }

 //oCPU.RunOpcode();
 //if (frame == 100000) 
 //{
//	oVIC->Update();
//	//JJ oHostIO.Yield();
//	frame = 0;
// }
// frame++;
// oCPU.RunOpcode();
}
