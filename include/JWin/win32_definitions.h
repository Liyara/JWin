#ifndef JWIN_WIN32_DEFINITIONS
#define JWIN_WIN32_DEFINITIONS

#include <JWin/jwin_internal.h>

#ifdef JUTIL_WINDOWS

#include <windows.h>
#include <windowsx.h>
#include <GL/wglew.h>

#define JWIN_TARGET_WINDOW	PFD_DRAW_TO_WINDOW
#define JWIN_TARGET_BITMAP  PFD_DRAW_TO_BITMAP

#define JWIN_RENDER_RGBA		PFD_TYPE_RGBA
#define JWIN_RENDER_COLORINDEX	PFD_TYPE_COLORINDEX

#define JWIN_DISPLAY_CONTEXT HDC
#define JWIN_CONTEXT HGLRC
#define JWIN_RAW_FBCONFIG int
#define JWIN_ID int
#define JWIN_PLATFORM_FORMAT __jwin_win32_platform_format__
#define JWIN_CORE_PROFILE WGL_CONTEXT_CORE_PROFILE_BIT_ARB
#define JWIN_COMPATIBILITY_PROFILE WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
#define ATTRIB_NOT_FOUND -0xFFA0987E
#define JWIN_EXTENSION_CREATE_CONTEXT_ATTRIBS #WGL_ARB_create_context

namespace jwin {
	namespace display_manager {
		/* This struct is currently unused, but structurally necessary */
		struct __jwin_win32_platform_format__ {};
	}
}

#endif
#endif
