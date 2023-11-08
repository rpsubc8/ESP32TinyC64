/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "vic.h"
#include "hostio.h"
#include <stdio.h>
#include <string.h>
#include "gb_globals.h"
#include "c64.h"
#include "gbConfig.h"

#if 1
#define TRACE(x...)
#else
#define TRACE(x...) printf(x)
#endif

VIC::VIC(CPU6502& oCPU, Memory& oMemory, HostIO& oHostIO)
	: XPeripheral(oCPU, oMemory), m_hostio(oHostIO)
{
	//JJ m_vic_regs = new uint8_t[Memory::s_vic_length];
//	m_vic_regs = (uint8_t*) malloc(Memory::s_vic_length);
}

VIC::~VIC()
{
	delete[] m_vic_regs;
}

//***************************************
void VIC::AssignPtrRegs(uint8_t *auxPtr)
{
 m_vic_regs= auxPtr;
}

void VIC::Reset()
{
	m_screen_base = 0x400;
	memset(m_vic_regs, 0, Memory::s_vic_length); /* XXX is this correct? */
}

void VIC::Poke(Memory::addr_t addr, uint8_t val)
{
	m_vic_regs[addr & 0xff] = val;

 //if ((addr >= 0xD800) && (addr <= 0xDBE7))
 //{
 // printf("Poke addr:0x%x val:%x\n", addr, val);
 //}	

	switch(addr & 0xff) {
		case 0x18: // memory control register
			m_screen_base = (val >> 4) * 0x400;
			m_char_base = (val & 0xe) * 0x400;
			//JJ TRACE("VIC::Poke(): mcr, val %x => screen_base=%x char_base=%x\n", val, m_screen_base, m_char_base);
			//printf("VIC::Poke(): mcr, val %x => screen_base=%x char_base=%x\n", val, m_screen_base, m_char_base);
			break;
		default:                            
		    #ifdef use_lib_log_serial
             if ((addr >= 0xD800) && (addr <= 0xDBE7))            
             {			 
              printf("VIC::Poke(): unhandled addr=0x%x val=%x\n", addr, val);
             }
			#endif
			//JJ TRACE("VIC::Poke(): unhandled addr=0x%x val=%x\n", addr, val);
			break;
	}
}

uint8_t VIC::Peek(Memory::addr_t addr)
{
	switch(addr & 0xff) {
		case 0x12: // read/write raster value
			return 0;
		default:
			//JJ TRACE("VIC::Peek(): unhandled addr=0x%x\n", addr);
			break;
	}
	return m_vic_regs[addr & 0xff];
}

static unsigned char gb_color_vga[16]={
 0x00,0x3F,0x01,0x29,0x21,0x19,0x10,0x1E,
 0x05,0x01,0x16,0x15,0x15,0x2E,0x25,0x2A 
};

void PreparaColorVGA()
{
 for (unsigned char i=0;i<16;i++) 
  gb_color_vga[i] = (gb_color_vga[i] & vga.RGBAXMask) | vga.SBits; 
}

//*********************************************
inline unsigned char GetModeVideo()
{
 unsigned char aMCM,aBMM,aECM,aReturn;
 aMCM= (gb_memory[0xD016]>>4) & 0x01;
 aBMM= (gb_memory[0xD011]>>5) & 0x01;
 aECM= (gb_memory[0xD011]>>6) & 0x01;
 aReturn= (aECM<<2)|(aBMM<<1)|aMCM;
 return aReturn;
}

//void VIC::Update()
void jj_video_Update()
{
 int x_offset=32;
 int y_offset=32; 
 int auxX,auxY;
 unsigned char jj_bg_color= gb_color_vga[(gb_memory[0xD021] & 0x0F)];
 unsigned char jj_border_color= gb_color_vga[(gb_memory[0xD020] & 0x0F)];
 int aux_cont=0xD800;
 int auxRow;
 //printf("Vicreg 0x21:%d\n",gb_vic_regs[0x21]); //Tambien sirve
 //printf("Vicreg 0x20:%d\n",gb_vic_regs[0x20]);

 //unsigned char video_mode= GetModeVideo();
 //printf("Modo video:%d\n",video_mode); //0 Standard Character Mode
 //printf("Video mode:%d\n",video_mode);
// printf("Backcolor:%d Border:%d\n",jj_bg_color,jj_border_color);

 for (int j=0;j<y_offset;j++)
 {//Borde arriba y abajo
  for (int i=0;i<(320+x_offset+x_offset);i++)
  {
   //vga.dotFast(i,j,jj_border_color);
   //vga.dotFast(i,(j+200+y_offset),jj_border_color);
   gb_buffer_vga[j][i^2]= jj_border_color;
   gb_buffer_vga[(j+200+y_offset)][i^2]= jj_border_color;
  }
 }
 for (int j=y_offset;j<(y_offset+200);j++)
 {//Borde izquierda y derecha
  for (int i=0;i<x_offset;i++)
  {
   //vga.dotFast(i,j,jj_border_color);
   //vga.dotFast((i+320+x_offset),j,jj_border_color);
   gb_buffer_vga[j][i^2]= jj_border_color;
   gb_buffer_vga[j][(i+320+x_offset)^2]= jj_border_color;
  }
 } 
 
 //printf("bg:%d border:%d\n",jj_bg_color,jj_border_color);
 //printf("addr_base=%x, screen_base=%x char_base=%x\n", m_addr_base, m_screen_base, m_char_base);
 for (int y = 0; y < 25; y++)
 {
  //printf ("\n");
  auxRow= 0x0400+(y<<5)+(y<<3); //(y*32)+(y*8)
  for (int x = 0; x < 40; x++)
  {
   //JJ uint8_t n = ByteAt(m_addr_base + m_screen_base + (y * 40) + x);   
   uint8_t n = gb_memory[(auxRow + x)];
   //unsigned char jj_color = gb_memory[((0xD800+ (y * 40) + x))];
   unsigned char jj_color = gb_color_vga[(gb_memory[aux_cont++] & 0x0F)];

   //printf ("%d ",jj_color);
   for (int j = 0; j < 8; j++)
   {
	for (int i = 0; i < 8; i++) 
    {
 	 //JJ uint8_t ch = ByteAt(m_addr_base + m_char_base + n * 8 + j);
	 //uint8_t ch = ByteAt(0xd000 + n * 8 + j); //Char ROM
	 unsigned char ch = gb_memory[(0xd000 + (n<<3) + j)];
	 //JJ m_hostio.putpixel(x * 8 + i, y * 8 + j, (ch & (1 << (8 - i))) ? 1 : 0);					
	 //auxX = ((x<<3) + i)+x_offset;
	 //auxY = ((y<<3) + j)+y_offset;
	 //m_hostio.putpixel(auxX, auxY, (ch & (1 << (8 - i))) ? jj_color : jj_bg_color);
	 auxX = (x<<3) + i+x_offset;
	 auxY = (y<<3) + j+y_offset;
	 //vga.dotFast(auxX,auxY, (ch & (1 << (8 - i))) ? jj_color : jj_bg_color);	 
	 gb_buffer_vga[auxY][auxX^2]= (ch & (1 << (8 - i))) ? jj_color : jj_bg_color;
	}
   }
  }
 }
 
 //printf("cont:%d\n",aux_cont-0xD800);
}

//vim:set ts=2 sw=2:
