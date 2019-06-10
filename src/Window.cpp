#include "JWin/Window.h"
#include <JUtil/IO/IO.h>

namespace jwin {

	Window::EventThread::EventThread(Window *p) : parent(p) {

	}
	void Window::EventThread::onPause() {
	}
	void Window::EventThread::onStop() {
	} 
	void Window::EventThread::onUnpause() {
	}
	void Window::EventThread::main() {
		Event e;
		e.type = Event::Type::NO_TYPE;
    	while (true) {
			e = input_manager::pollEvent(parent->handle, &(parent->geometry));
			if (e.type != Event::Type::NO_TYPE) {
				if (e.type == Event::Type::CLOSE) parent->close();
				parent->handleEvent(e);
			}
			if (!window_manager::windowIsRegistered(parent->handle)) break;
			yield();
    	}
	}

	Window::Window(const jutil::String &n, const Dimensions &d, const Position &p, const Monitor *m) : name(n), eventThread(this) {
		if (!m) m = display_manager::getPrimaryMonitor();
		geometry.position = display_manager::monitorToDisplay(m, p);
		geometry.size = d;
		handle = window_manager::registerWindow(m, name, d, p);
		eventThread.start();
	}

	void Window::setSize(const Dimensions &d) {
		window_manager::resizeWindow(handle, d);
	}

	void Window::setPosition(const Position &p) {
		window_manager::moveWindow(handle, p, geometry);
	}

	void Window::maximize() {
		window_manager::issueCommands(handle, window_manager::MAXIMIZE, window_manager::WindowAction::SET);
	}
	void Window::fullscreen(unsigned action) {
		window_manager::issueCommands(handle, window_manager::FULLSCREEN, (window_manager::WindowAction)action);
	}
	void Window::minimize(unsigned action) {
		window_manager::issueCommands(handle, window_manager::MINIMIZE, (window_manager::WindowAction)action);
	}
	void Window::hideInTaskbar(unsigned action) {
		window_manager::issueCommands(handle, window_manager::HIDE_TASKBAR, (window_manager::WindowAction)action);
	}
	void Window::allowMove(unsigned action) {
		window_manager::issueCommands(handle, window_manager::MOVABLE, (window_manager::WindowAction)action);
	}
	void Window::allowResize(unsigned action) {
		window_manager::issueCommands(handle, window_manager::RESIZABLE, (window_manager::WindowAction)action);
	}
	void Window::showBorder(unsigned action) {
		window_manager::issueCommands(handle, window_manager::BORDER, (window_manager::WindowAction)action);
	}

	void Window::sendAlert() {
		window_manager::issueCommands(handle, window_manager::ALERT, window_manager::WindowAction::SET);
	}
	void Window::onTop(unsigned action) {
		window_manager::issueCommands(handle, window_manager::TOP, (window_manager::WindowAction)action);
	}

	void Window::vsync(bool sync) {
		display_manager::setVSync(handle, sync);
	}
	void Window::swapBuffers() {
		display_manager::swapBuffers(handle);
	}

	bool Window::alertActive() const {
		return window_manager::isSet(handle, window_manager::ALERT);
	}
	bool Window::maximized() const {
		return window_manager::isSet(handle, window_manager::MAXIMIZE);
	}
	bool Window::fullscreened() const {
		return window_manager::isSet(handle, window_manager::FULLSCREEN);
	}
	bool Window::minimized() const {
		return window_manager::isSet(handle, window_manager::MINIMIZE);
	}
	bool Window::visibleInTaskbar() const {
		return !window_manager::isSet(handle, window_manager::HIDE_TASKBAR);
	}
	bool Window::isOnTop() const {
		return window_manager::isSet(handle, window_manager::TOP);
	}
	bool Window::canMove() const {
		return window_manager::isSet(handle, window_manager::MOVABLE);
	}
	bool Window::canResize() const {
		return window_manager::isSet(handle, window_manager::RESIZABLE);
	}
	bool Window::borderShown() const {
		return window_manager::isSet(handle, window_manager::BORDER);
	}

	void Window::close() {
		eventThread.tellStop();
		window_manager::unregisterWindow(handle);
	}

	void Window::handleEvent(const Event&) {

	}

	bool Window::open() {
		return window_manager::windowIsRegistered(handle);
	}

	const jutil::String &Window::getName() const {
		return name;
	}
		
	const Dimensions &Window::getSize() const {
		return geometry.size;
	}
	const Position &Window::getPosition() const {
		return geometry.position;
	}

	Window::~Window() {
		close();
	}
}