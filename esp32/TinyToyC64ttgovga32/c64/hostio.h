#ifndef __HOSTIO_H__
#define  __HOSTIO_H__

//JJ #include <SDL/SDL.h>
#include <map>
#include <vector>

class HostIO
{
public:
	HostIO();

	bool Initialize();
	void Cleanup();
	void Yield();
	void putpixel(unsigned int x, unsigned int y, unsigned int c);

	bool Quitting() const { return m_quitting; }

	typedef void (*TKeyCallback)(void*, int, bool);
	void SetKeyCallback(TKeyCallback callback, void* arg);
	unsigned int KeyboardFillCallback();

	void fill(int* values);

protected:
	//JJ SDL_Surface* m_screen;
	//JJ SDL_Surface* m_display;

	//JJ typedef std::map<SDLKey, int> TSDLKeyIntMap;
	//JJ TSDLKeyIntMap m_kbdmap;

	//JJ void HandleKey(SDLKey key, bool pressed);

	TKeyCallback m_keycallback;
	void* m_keycallback_arg;

	//JJ std::vector<uint32_t> m_color; //No lo necesito

	int* m_fill_buffer;
	bool m_fill_first;

	//! \brief Amount of ms between each fill character
	static const int s_fill_delay = 50;
	
	bool m_quitting;
};

#endif /*   __HOSTIO__ */
