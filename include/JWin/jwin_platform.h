#ifndef JWIN_PLATFORM
#define JWIN_PLATFORM

#include <JWin/jwin.h>

/*
	Defines functions and data structures used on all platforms.
*/

namespace jwin {
	namespace display_manager {
		//Data structure describing monitors connected to the system
		struct MonitorData {
			jutil::Queue<Monitor*> monitors;
			Monitor *primaryMonitor;
			MonitorData() :
				primaryMonitor(nullptr)
			{}
		};

		extern MonitorData monitorData;

		bool createDisplay();
		void destroyDisplay();
	}

	namespace window_manager {
		/*
			List of Handles referncing currently active windows.

			Attempting to pass unregistered Handles to JWin functions
			will result in errors.
		*/
		extern jutil::Queue<Handle> registeredWindows;

		bool init();
		void terminate();
	}

	namespace input_manager {
		bool init();
		void terminate();
	}

	namespace _dmg = display_manager;
	namespace _wmg = window_manager;
	namespace _img = input_manager;
}

#endif