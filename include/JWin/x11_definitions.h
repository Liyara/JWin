#ifndef JWIN_X11_DEFINITIONS
#define JWIN_X11_DEFINITIONS

#include <JWin/jwin_internal.h>

#ifdef JUTIL_LINUX

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <X11/XKBlib.h>
#include <GL/glx.h>

#define JWIN_TARGET_WINDOW	GLX_WINDOW_BIT
#define JWIN_TARGET_BITMAP  GLX_PIXMAP_BIT

#define JWIN_RENDER_RGBA		GLX_RGBA_BIT
#define JWIN_RENDER_COLORINDEX	GLX_COLOR_INDEX_BIT

#define JWIN_DISPLAY_CONTEXT Display*
#define JWIN_CONTEXT GLXContext
#define JWIN_RAW_FBCONFIG GLXFBConfig
#define JWIN_ID XID
#define JWIN_PLATFORM_FORMAT __jwin_x11_platform_format__
#define JWIN_CORE_PROFILE GLX_CONTEXT_CORE_PROFILE_BIT_ARB
#define JWIN_COMPATIBILITY_PROFILE GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
#define JWIN_EXTENSION_CREATE_CONTEXT_ATTRIBS #GLX_ARB_create_context

namespace jwin {
	namespace display_manager {
		struct __jwin_x11_platform_format__ {
			XID vid;
			XRenderPictFormat *format;
			XVisualInfo *visualInfo;
			GLint visualType;
			GLXFBConfig glxConfig;
			GLint bufferLevel; 
			__jwin_x11_platform_format__() :
				vid(0),
				format(nullptr),
				visualInfo(nullptr),
				visualType(GLX_TRUE_COLOR),
				bufferLevel(0)
			{}
		};
	}
}

#endif
#endif