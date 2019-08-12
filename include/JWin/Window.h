#ifndef JWIN_WINDOW_H
#define JWIN_WINDOW_H

#include <JWin/jwin.h>
#include <JUtil/Core/Thread.h>

namespace jwin {

	class Window : public jutil::NonCopyable {
	public:
		Window(const jutil::String&, const Dimensions&, const Position& = {0, 0}, const Monitor* = nullptr);
		const jutil::String &getName() const;
		const Dimensions &getSize() const;
		const Position &getPosition() const;
		void setSize(const Dimensions&);
		void setPosition(const Position&);
		void close();
		bool open();
		void sendAlert();
		void maximize();
		void fullscreen(unsigned = 1);
		void minimize(unsigned = 1);
		void hideInTaskbar(unsigned = 1);
		void vsync(bool);
		void swapBuffers();
		void onTop(unsigned = 1);

		bool alertActive() const;
		bool maximized() const;
		bool fullscreened() const;
		bool minimized() const;
		bool visibleInTaskbar() const;
		bool isOnTop() const;
		bool canMove() const;
		bool canResize() const;
		bool borderShown() const;

		//On Linux, Requires a window manager which respects _MOTIF_WM_HINTS
		void allowMove(unsigned = 1);
		void allowResize(unsigned = 1);
		void showBorder(unsigned = 1);

		virtual ~Window();
	private:	
		jutil::String name;
		Geometry geometry;
		Handle handle;
		ContextID contextID;
		const Monitor *initMonitor;

		struct EventThread : public jutil::Thread {
			EventThread(Window*);
			void onPause() override;
			void onStop() override;
			void onUnpause() override;
			void main() override;

			Window *parent;

		};

		EventThread eventThread;

	protected:
		virtual void handleEvent(const Event&);
	};
}

#endif