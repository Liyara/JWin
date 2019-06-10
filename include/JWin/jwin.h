#ifndef JWIN_DISPLAY_H
#define JWIN_DISPLAY_H

/*
	This header containes the public, user-facing side of the JWin library.
*/

#include <JUtil/Container/Queue.hpp>
#include <JWin/Monitor.h>
#include <JWin/Event.h>

#ifdef SET_DISPLAY_ADAPTOR
	#define JWIN_SET_DISPLAY_ADAPTER SET_DISPLAY_ADAPTOR
#endif

namespace jwin {

	void init();
	void terminate();

	enum ContextHint {
		JWIN_SAMPLES,
		JWIN_TRANSPARENT,
		JWIN_MASK_ACCUM,
		JWIN_MASK_DS,
		JWIN_STEREO,
		JWIN_GL_VERSION_MAJOR,
		JWIN_GL_VERSION_MINOR
	};

	void setContextHintPriority(ContextHint, float);

	/*
		Used to specify properties of the OpenGL context created by JWIn.

		NOTE: You must call this function BEFORE calling init()!
	*/
	bool setContextHint(int, int);
	bool setContextHint(int, bool);
	bool setContextHint(int, unsigned);

	namespace window_manager {

		/*
			Window properties controllable in real-time
			using the issueCommands() function.
		*/
		enum WindowProperty {
			MAXIMIZE = 0x01, //SET OONLY
			MINIMIZE = 0x02,
			FULLSCREEN = 0x04,
			HIDE_TASKBAR = 0x08,
			ALERT = 0x10,
			MOVABLE = 0x20,
			RESIZABLE = 0x40,
			BORDER = 0x80,
			TOP = 0x100
		};

		/*
			Used to inform issueCommands() how the specified
			properties should be altered.
		*/
		enum WindowAction {
			UNSET,
			SET,
			TOGGLE
		};

		/*
			Create a JWin window and registeres it with the window manager.

			Registered windows are closely tracked by JWin and registration
			is a prerequisite for use internally.
		*/
		Handle registerWindow(const Monitor*, const jutil::String&, const Dimensions&, const Position& = Position {0, 0});

		/*
			Destroys a window and untracks it.

			If the given window handle is registered,
			it will be destroyed and then set null.

			Otherwise, the request is ignored and nothing happens.
		*/
		bool unregisterWindow(Handle);

		//	Return if the given window mandle is registered to JWin's window manager.
		bool windowIsRegistered(Handle);

		/*
			Issue commands to control window behavior in real-time.
			
			NOTE: issuing commands from seperate threads may cause slow-downs,
			as only one thread at a time may edit a window's properties.
		*/
		bool issueCommands(Handle, unsigned, WindowAction);
		bool moveWindow(Handle, Position, const Geometry&);
		bool resizeWindow(Handle, const Dimensions&);
		bool toggleStatus(Handle, unsigned);
		bool isSet(Handle, WindowProperty);
	}

	namespace input_manager {
		const Position &getCursorPosition(Handle = nullptr);

		/*
			Retrieve the current action of the key.
			Will be either DOWN or UP.
		*/
		const Event::Action &keyState(const Event::Key&);

		/*
			Retrieve an event sent to the window specified by the handle.

			The event argument specifies the last event that was retrieved.
			If there was no previous event, simply pass an event
			with type NO_TYPE

			The geometry pointer specifies the window's current gewometry.
			This is used to detect geometry changes and will be updates to
			reflect the window's new geometry. Passing nullptr for this argument
			will cause pollevent to be unable to detect RESIZE or MOVE events.
		*/
		Event pollEvent(Handle, Geometry* = nullptr);
		void setInputMode(InputMode);
	}

	namespace display_manager {
		const jutil::Queue<Monitor*> &getMonitors();
		const Monitor *getMonitor(size_t);
		const Monitor *getPrimaryMonitor();
		const Monitor *pointInMonitor(const Position&);
		Position monitorToDisplay(const Monitor*, const Position&);
		Position displayToMonitor(const Position&, const Monitor** = nullptr);
		void setVSync(Handle, bool);
		void swapBuffers(Handle);
	}
}

#endif