/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "hostio.h"
//JJ #include <SDL/SDL.h>
#include <assert.h>
#include "cpu6502.h"
#include "vic.h"
#include "gb_globals.h"
#include "c64.h"

#define S_SHIFT 0x100
#define S_END -1

// LOAD"*",8,1
static int s_load[] = {
	42, 38, 10, 18, S_SHIFT | 59, 49, S_SHIFT | 59, 47, 27, 47, 56, 1, S_END
};

HostIO::HostIO()
{
	m_quitting = false;
	m_keycallback = NULL;
	m_keycallback_arg = NULL;
	m_fill_buffer = NULL;

 /*JJ
	// row 0
	m_kbdmap[SDLK_BACKSPACE] = 0;
	m_kbdmap[SDLK_RETURN] = 1;
	m_kbdmap[SDLK_RIGHT] = 2; // cursor left/right
	m_kbdmap[SDLK_F7] = 3;
	m_kbdmap[SDLK_F1] = 4;
	m_kbdmap[SDLK_F3] = 5;
	m_kbdmap[SDLK_F5] = 6;
	m_kbdmap[SDLK_DOWN] = 7;
	// row 1
	m_kbdmap[SDLK_3] = 8;
	m_kbdmap[SDLK_w] = 9;
	m_kbdmap[SDLK_a] = 10;
	m_kbdmap[SDLK_4] = 11;
	m_kbdmap[SDLK_z] = 12;
	m_kbdmap[SDLK_s] = 13;
	m_kbdmap[SDLK_e] = 14;
	m_kbdmap[SDLK_LSHIFT] = 15;
	// row 2
	m_kbdmap[SDLK_5] = 16;
	m_kbdmap[SDLK_r] = 17;
	m_kbdmap[SDLK_d] = 18;
	m_kbdmap[SDLK_6] = 19;
	m_kbdmap[SDLK_c] = 20;
	m_kbdmap[SDLK_f] = 21;
	m_kbdmap[SDLK_t] = 22;
	m_kbdmap[SDLK_x] = 23;
	// row 3
	m_kbdmap[SDLK_7] = 24;
	m_kbdmap[SDLK_y] = 25;
	m_kbdmap[SDLK_g] = 26;
	m_kbdmap[SDLK_8] = 27;
	m_kbdmap[SDLK_b] = 28;
	m_kbdmap[SDLK_h] = 29;
	m_kbdmap[SDLK_u] = 30;
	m_kbdmap[SDLK_v] = 31;
	// row 4
	m_kbdmap[SDLK_9] = 32;
	m_kbdmap[SDLK_i] = 33;
	m_kbdmap[SDLK_j] = 34;
	m_kbdmap[SDLK_0] = 35;
	m_kbdmap[SDLK_m] = 36;
	m_kbdmap[SDLK_k] = 37;
	m_kbdmap[SDLK_o] = 38;
	m_kbdmap[SDLK_n] = 39;
	// row 5
	m_kbdmap[SDLK_PLUS] = 40;
	m_kbdmap[SDLK_p] = 41;
	m_kbdmap[SDLK_l] = 42;
	m_kbdmap[SDLK_MINUS] = 43;
	m_kbdmap[SDLK_PERIOD] = 44;
	m_kbdmap[SDLK_COLON] = 45;
	m_kbdmap[SDLK_F8]= 45; //F8 escribe :
	m_kbdmap[SDLK_AT] = 46;
	m_kbdmap[SDLK_COMMA] = 47;
	// row 6
	//m_kbdmap[SDLK_] = 48; // XXX pound
	m_kbdmap[SDLK_LEFTBRACKET] = 49; // asterisk
	m_kbdmap[SDLK_SEMICOLON] = 50;
	m_kbdmap[SDLK_HOME] = 51;
	m_kbdmap[SDLK_RSHIFT] = 52;
	m_kbdmap[SDLK_EQUALS] = 53;
	m_kbdmap[SDLK_UP] = 54;
	m_kbdmap[SDLK_SLASH] = 55;
	// row 7
	m_kbdmap[SDLK_1] = 56;
	m_kbdmap[SDLK_LEFT] = 57;
	m_kbdmap[SDLK_LCTRL] = 58;
	m_kbdmap[SDLK_2] = 59;
	m_kbdmap[SDLK_SPACE] = 60;
	m_kbdmap[SDLK_LALT] = 61; // commodore
	m_kbdmap[SDLK_q] = 62;
	m_kbdmap[SDLK_TAB] = 63; // run/stop
*/	

	// colours, from http://unusedino.de/ec64/technical/misc/vic656x/colors/
	//JJ m_color.push_back(0x000000);
	//JJ m_color.push_back(0xffffff);
	//JJ m_color.push_back(0x68372b);
	//JJ m_color.push_back(0x70a4b2);
	//JJ m_color.push_back(0x6f3d86);
	//JJ m_color.push_back(0x588d43);
	//JJ m_color.push_back(0x352879);
	//JJ m_color.push_back(0xb8c76f);
	//JJ m_color.push_back(0x6f4f25);
	//JJ m_color.push_back(0x433900);
	//JJ m_color.push_back(0x9a6759);
	//JJ m_color.push_back(0x444444);
	//JJ m_color.push_back(0x6c6c6c);
	//JJ m_color.push_back(0x9ad284);
	//JJ m_color.push_back(0x6c5eb5);
	//JJ m_color.push_back(0x959595);
}

//JJ SDL_Color gb_colors[256];

bool HostIO::Initialize()
{
//JJ	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
//JJ		return false;

   for (int i=0;i<256;i+=16)
   {  
   /* 
    gb_colors[0+i].b = 0; gb_colors[0+i].g = 0; gb_colors[0+i].r= 0;
    gb_colors[1+i].b = 0; gb_colors[1+i].g = 0; gb_colors[1+i].r= 255;
	gb_colors[2+i].b = 0; gb_colors[2+i].g = 255; gb_colors[2+i].r= 0;
    gb_colors[3+i].b = 0; gb_colors[3+i].g = 255; gb_colors[3+i].r= 255;
	gb_colors[4+i].b = 255; gb_colors[4+i].g = 0; gb_colors[4+i].r= 0;
    gb_colors[5+i].b = 255; gb_colors[5+i].g = 0; gb_colors[5+i].r= 255;
    gb_colors[6+i].b = 255; gb_colors[6+i].g = 255; gb_colors[6+i].r= 0;
    gb_colors[7+i].b = 255; gb_colors[7+i].g = 255; gb_colors[7+i].r= 255;
    */
   /*JJ
    gb_colors[0+i].r = 0x00; gb_colors[0+i].g = 0x00; gb_colors[0+i].r= 0x00;
    gb_colors[1+i].r = 0xFF; gb_colors[1+i].g = 0xFF; gb_colors[1+i].b= 0xFF;
	gb_colors[2+i].r = 0x68; gb_colors[2+i].g = 0x37; gb_colors[2+i].b= 0x2B;
    gb_colors[3+i].r = 0x70; gb_colors[3+i].g = 0xA4; gb_colors[3+i].b= 0xB2;
	gb_colors[4+i].r = 0x6F; gb_colors[4+i].g = 0x3D; gb_colors[4+i].b= 0x86;
    gb_colors[5+i].r = 0x58; gb_colors[5+i].g = 0x8D; gb_colors[5+i].b= 0x43;
    gb_colors[6+i].r = 0x35; gb_colors[6+i].g = 0x28; gb_colors[6+i].b= 0x79;
    gb_colors[7+i].r = 0xB8; gb_colors[7+i].g = 0xC7; gb_colors[7+i].b= 0x6F;
    gb_colors[8+i].r = 0x6F; gb_colors[8+i].g = 0x4F; gb_colors[8+i].b= 0x25;
    gb_colors[9+i].r = 0x43; gb_colors[9+i].g = 0x39; gb_colors[9+i].b= 0x00;
	gb_colors[10+i].r = 0x9A; gb_colors[10+i].g = 0x67; gb_colors[10+i].b= 0x59;
    gb_colors[11+i].r = 0x44; gb_colors[11+i].g = 0x44; gb_colors[11+i].b= 0x44;
	gb_colors[12+i].r = 0x6C; gb_colors[12+i].g = 0x6C; gb_colors[12+i].b= 0x6C;
    gb_colors[13+i].r = 0x9A; gb_colors[13+i].g = 0xD2; gb_colors[13+i].b= 0x84;
    gb_colors[14+i].r = 0x6C; gb_colors[14+i].g = 0x5E; gb_colors[14+i].b= 0xB5;
    gb_colors[15+i].r = 0x95; gb_colors[15+i].g = 0x95; gb_colors[15+i].b= 0x95;
	*/
   }
   
   for (int i=0;i<256;i++)
   {//Adapto a 2 bits RRGGBB de color
    /*JJ
	gb_colors[i].r= (((gb_colors[i].r>>6)&0x03)<<6);
    gb_colors[i].g= (((gb_colors[i].g>>6)&0x03)<<6);
    gb_colors[i].b= (((gb_colors[i].b>>6)&0x03)<<6);
	*/
   }
   

	//JJ m_screen = SDL_SetVideoMode(VIC::s_video_width, VIC::s_video_height, 32, SDL_SWSURFACE);
	//m_screen = SDL_SetVideoMode(VIC::s_video_width, VIC::s_video_height, 8, SDL_HWSURFACE | SDL_DOUBLEBUF);
//JJ	m_screen = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_DOUBLEBUF);
//JJ    SDL_WM_SetCaption("SDL Port ESP32 C64",NULL);
//JJ	SDL_SetPalette(m_screen,SDL_LOGPAL|SDL_PHYSPAL, gb_colors, 0, 256);
//JJ	if (m_screen == NULL)
//JJ		return false;
	//JJ assert(m_screen->format->BytesPerPixel == 4);

	//JJ m_display = SDL_CreateRGBSurface(SDL_SWSURFACE, VIC::s_video_width, VIC::s_video_height, 32, 0, 0, 0, 0);
    //JJ m_display = SDL_CreateRGBSurface(SDL_SWSURFACE, VIC::s_video_width, VIC::s_video_height, 8, 0, 0, 0, 0);
//JJ    m_display = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 8, 0, 0, 0, 0);
//JJ    SDL_SetPalette(m_display,SDL_LOGPAL|SDL_PHYSPAL, gb_colors, 0, 256);
//JJ	if (m_display == NULL)
//JJ		return false;

//JJ	SDL_Rect r;
//JJ	r.x = 0; r.y = 0;
//JJ	r.h = VIC::s_video_height; r.w = VIC::s_video_width;
//JJ	SDL_FillRect(m_display, &r, SDL_MapRGB(m_display->format, 100, 100, 100));

	return true;
}

void HostIO::Cleanup()
{
//JJ	SDL_FreeSurface(m_display);
//JJ	SDL_Quit();
}

void HostIO::Yield()
{
//JJ	SDL_BlitSurface(m_display, NULL, m_screen, NULL);
//JJ	SDL_Flip(m_screen);

/*JJ
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				m_quitting = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_INSERT) {
					fill(s_load);
				}
				if (event.key.keysym.sym == SDLK_BACKSLASH) {
					extern CPU6502 oCPU;
					oCPU.SetTrace(true);
				}
				HandleKey(event.key.keysym.sym, true);
				break;
			case SDL_KEYUP:
				HandleKey(event.key.keysym.sym, false);
				break;
		}
	}
	*/
}

/*JJ
void HostIO::SetKeyCallback(TKeyCallback callback, void* arg)
{
	m_keycallback = callback;
	m_keycallback_arg = arg;
}
*/

/*JJ
void HostIO::HandleKey(SDLKey key, bool pressed)
{
	TSDLKeyIntMap::iterator it = m_kbdmap.find(key);
	if (it == m_kbdmap.end())
		return;

	if (m_keycallback != NULL)
		m_keycallback(m_keycallback_arg, it->second, pressed);
}
*/

void HostIO::putpixel(unsigned int x, unsigned int y, unsigned int c)
{
//JJ	if (x >= (unsigned int)VIC::s_video_width || y >= (unsigned int)VIC::s_video_height)
//JJ		return;
//JJ	if (x >= 400 || y >= 300)
//JJ		return;
	//JJ assert(c >= 0 && c < m_color.size());

	//JJ Uint32* p = (Uint32*)m_display->pixels + y * m_display->pitch/4 + x;	
	//JJ *p = m_color[c];
//JJ	Uint8* p = (Uint8*)m_display->pixels + (y * m_display->pitch) + x;
//JJ	*p= c;
 vga.dot(x,y,c);
}

//JJ static Uint32 kbd_timer_callback(Uint32 interval, void* param)
//JJ {
//JJ 	return ((HostIO*)param)->KeyboardFillCallback();
//JJ }

unsigned int HostIO::KeyboardFillCallback()
{
	// Mark current key as unpressed, if this isn't our first call
	if (!m_fill_first) {
		int cur_key = *m_fill_buffer;
		if (cur_key & S_SHIFT)
			m_keycallback(m_keycallback_arg, 15 /* LSHIFT */, false);
		m_keycallback(m_keycallback_arg, cur_key & ~S_SHIFT, false);

		// Advance pointer; it was pointing to the current char
		m_fill_buffer++;
	}
	m_fill_first = false;

	// Fetch next key; abort if it's the end of the line
	int next_key = *m_fill_buffer;
	if (next_key == S_END)
		return 0;

	// Process next key and re-schedule timer function
	if (next_key & S_SHIFT)
		m_keycallback(m_keycallback_arg, 15 /* LSHIFT */, true);
	m_keycallback(m_keycallback_arg, next_key & ~S_SHIFT, true);
	return s_fill_delay;
}

void HostIO::fill(int* values)
{
	m_fill_buffer = values;
	m_fill_first = true;
	//JJ SDL_AddTimer(s_fill_delay, kbd_timer_callback, this);
}

/* vim:set ts=2 sw=2: */
