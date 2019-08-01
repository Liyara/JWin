#ifndef JWIN_INTERNAL_H
#define JWIN_INTERNAL_H

#include <JWin/jwin.h>
#include <JUtil/Core/version.h>
#include <JUtil/Core/Pair.hpp>
#include <GL/glew.h>

#define JWIN_BITS_32 	0x08080808
#define JWIN_BITS_64 	0x10101010
#define JWIN_RGBA 		JWIN_BITS_32
#define JWIN_ACCUM_64	JWIN_BITS_64
#define JWIN_ACCUM_32	JWIN_BITS_32
#define JWIN_RGB 		0x08080800
#define JWIN_DS_32 		0x00180008
#define JWIN_DS_24		0x00100008
#define JWIN_D_24		0x00180000
#define JWIN_D_16		0x00100000
#define JWIN_S_8		0x00000008

#define JWIN_TARGET_NULL	0
#define JWIN_RENDER_NULL	0

namespace jwin {
	namespace display_manager {

	}
}

#endif