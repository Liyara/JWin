#ifndef JWIN_WIN32_H
#define JWIN_WIN32_H
#include <JUtil/Core/version.h>
#ifdef JUTIL_WINDOWS
#include <JWin/jwin_platform.h>
#include <windows.h>

namespace jwin {

	namespace window_manager {

		extern WNDCLASSEX windowClass;
		bool registerClass();
	}

	namespace input_manager {

		struct InternalEvent {
			Event event;
			Handle window;
			UINT msg;
			WPARAM wp;
			LPARAM lp;
			InternalEvent() :
				window(nullptr),
				msg(0),
				wp(0),
				lp(0)
			{
				event.type = Event::Type::NO_TYPE;
			}
		};

		LRESULT CALLBACK eventProc(HWND, UINT, WPARAM, LPARAM);
		extern jutil::Queue<InternalEvent> eventQueue;
		bool hasEvents(Handle);
		InternalEvent popEvent(Handle);
		void internalEventHandler(Handle);
	}

	namespace display_manager {
		int CALLBACK cbMonitorInfo(HMONITOR__*, HDC__*, tagRECT*, long int);
	}
}

#endif
#endif