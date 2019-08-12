#include <JWin/jwin_win32.h>
#ifdef JUTIL_WINDOWS

#include <JUtil/IO/IO.h>

namespace jwin {

	JWIN_CREATE_PROC_FORWARD(CreateContextAttribs);
	JWIN_CREATE_PROC_FORWARD(SwapIntervalEXT);
	JWIN_CREATE_PROC_FORWARD(GetPixelFormatAttribiv);
	JWIN_CREATE_PROC_FORWARD(GetExtensionsString);

	void init() {
		if (_dmg::createDisplay()) {
			_img::init();
			_wmg::init();
		}
	}

	bool fwininit = false;

	void win32init(Handle handle) {
		//Grab processes

		PIXELFORMATDESCRIPTOR pfd;
	    HGLRC tempContext;
	    HDC tempDC;

	    ZeroMemory(&pfd, sizeof(pfd));
	    pfd.nSize = sizeof(pfd);
	    pfd.nVersion = 1;
	    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	    pfd.iPixelType = PFD_TYPE_RGBA;
	    pfd.cColorBits = 24;

	    tempDC = GetDC((HWND)handle);

	    SetPixelFormat(tempDC, ChoosePixelFormat(tempDC, &pfd), &pfd);

	    tempContext = wglCreateContext(tempDC);
	    wglMakeCurrent(tempDC, tempContext);

	    jutil::out << "Creating processes." << jutil::endl;

	    JWIN_CREATE_PROC(CreateContextAttribs, wglCreateContextAttribsARB);
		JWIN_CREATE_PROC(SwapIntervalEXT, wglSwapIntervalEXT);
		JWIN_CREATE_PROC(GetPixelFormatAttribiv, wglGetPixelFormatAttribivARB);
		JWIN_CREATE_PROC(GetExtensionsString, wglGetExtensionsStringARB);

	    wglDeleteContext(tempContext);

	    fwininit = true;
	}

	void terminate() {
		_wmg::terminate();
		_img::terminate();
		_dmg::destroyDisplay();
	}

	void setContextHintPriority(ContextHint, float) {}

	bool setContextHint(int, int) {}
	bool setContextHint(int, bool) {}
	bool setContextHint(int, unsigned) {}

	namespace window_manager {

		WNDCLASSEX windowClass;
		jutil::Queue<Handle> registeredWindows;

		bool init() {
			return registerClass();
		}
		void terminate() {
			for (auto &i: registeredWindows) unregisterWindow(i);
		}

		bool registerClass() {

			ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			windowClass.lpfnWndProc = _img::eventProc;
			windowClass.hInstance = GetModuleHandle(NULL);
		    windowClass.lpszClassName = "JWin Window Class";

			windowClass.hIcon 	= LoadIcon (NULL, IDI_APPLICATION);
		    windowClass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
		    windowClass.hCursor = LoadCursor (NULL, IDC_ARROW);
		    
		    return RegisterClassEx(&windowClass);
		}

		Handle registerWindow(const Monitor *monitor, const jutil::String &title, const Dimensions &size, const Position &position) {

			jutil::out << "2A" << jutil::endl;

			jml::Vector<int, 2> realSize = static_cast<jml::Vector<int, 2> >(size);
			jml::Vector<int, 2> realPosition = 
				(monitor->getPosition() + (static_cast<jml::Vector<int, 2> >(monitor->getSize()) / 2) + position) - (realSize / 2)
			;

			char *ctitle = new char[title.size() + 1];
			title.array(ctitle);

			HWND win;

		    win = CreateWindowEx(
		       	WS_EX_CLIENTEDGE,
		        windowClass.lpszClassName,
		        ctitle,
		        WS_OVERLAPPEDWINDOW,
		        realPosition.x(),
		        realPosition.y(),
		       	realSize.x(),
		        realSize.y(),
		        HWND_DESKTOP,
		        NULL,
		       	windowClass.hInstance,
		        NULL
		    );

		    jutil::out << "2B" << jutil::endl;

		    delete[] ctitle;

		    if (win) {

			    ShowWindow(win, SW_SHOW);

			    registeredWindows.insert(win);
			}

		    return win;
		}


		bool unregisterWindow(Handle handle) {
			for (auto &i: registeredWindows) {
				if (i == handle) {
					registeredWindows.erase(&i);
					DestroyWindow((HWND)handle);
					return true;
				}
			}
			return false;
		}

		bool windowIsRegistered(Handle handle) {
			return registeredWindows.find(handle);
		}

		bool issueCommands(Handle, unsigned, WindowAction) {}
		bool moveWindow(Handle, Position, const Geometry&) {}
		bool resizeWindow(Handle, const Dimensions&) {}
		bool toggleStatus(Handle, unsigned) {}
		bool isSet(Handle, WindowProperty) {}
	}

	namespace input_manager {
		jutil::Queue<InternalEvent> eventQueue;

		bool init() {return true;}
		void terminate() {
			eventQueue.clear();
		}

		const Position &getCursorPosition(Handle) {}

		const Event::Action &keyState(const Event::Key&) {}

		bool hasEvents(Handle handle) {
			for (auto &i: eventQueue) {
				if (i.window == handle) {
					return true;
				}
			}
			return false;
		}

		InternalEvent popEvent(Handle handle) {
			InternalEvent *r = nullptr;
			for (InternalEvent *it = eventQueue.begin(); it != eventQueue.end();) {
				if (it->window == handle) {
					if (it->event.type == Event::Type::NO_TYPE) {
						eventQueue.erase(it);
					} else {
						if (!r) {
							r = it;
							eventQueue.erase(it);
						} else ++it;
					}	
				} else ++it;
			}
			if (!r) {
				return InternalEvent();
			} else return *r;
		}

		void internalEventHandler(InternalEvent e) {
			//DefWindowProc(e.window, e.msg, e.wp, e.lp)
		}

		Event pollEvent(Handle handle, Geometry* geo) {
			if (!hasEvents(handle)) {
				MSG msg;
				if (PeekMessage(&msg, (HWND)handle, 0, 0, PM_REMOVE)) {
			        DispatchMessage(&msg);
			        return pollEvent(handle, geo);
				} else {
					return InternalEvent().event;
				}
			} else {
				InternalEvent e = popEvent(handle);
				internalEventHandler(e);
				return e.event;
			}
		}

		void setInputMode(InputMode) {}

		LRESULT CALLBACK eventProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

			InternalEvent e;
			e.window = hwnd;
			e.msg = msg;
			e.wp = wp;
			e.lp = lp;
			LRESULT r = 0;

			switch(msg) {
				case WM_CLOSE:
					e.event.type = Event::Type::CLOSE;
					break;

				case WM_DESTROY:
					PostQuitMessage(0);
					return 0;
					break;

				case WM_LBUTTONDOWN:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::LEFT;
					e.event.action = Event::Action::DOWN;
					break;

				case WM_LBUTTONUP:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::LEFT;
					e.event.action = Event::Action::UP;
					break;

				case WM_RBUTTONDOWN:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::RIGHT;
					e.event.action = Event::Action::DOWN;
					break;

				case WM_RBUTTONUP:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::RIGHT;
					e.event.action = Event::Action::UP;
					break;

				case WM_MBUTTONDOWN:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::MIDDLE;
					e.event.action = Event::Action::DOWN;
					break;

				case WM_MBUTTONUP:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::MIDDLE;
					e.event.action = Event::Action::UP;
					break;

				case WM_KEYDOWN:
					e.event.type = Event::Type::KEY;
					e.event.code = 0;
					e.event.action = Event::Action::DOWN;
					break;

				case WM_KEYUP:
					e.event.type = Event::Type::KEY;
					e.event.code = 0;
					e.event.action = Event::Action::UP;
					break;

				case WM_SIZE:
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
        			UpdateWindow(hwnd);
					break;

				default: return DefWindowProc(hwnd, msg, wp, lp);
			}

			eventQueue.insert(e);

			return r;
		}
	}

	namespace display_manager {

		const jutil::Queue<int> ATTRIB_NAMES = {
			WGL_DRAW_TO_WINDOW_ARB,
			WGL_DRAW_TO_BITMAP_ARB,
			WGL_TRANSPARENT_ARB,
			WGL_SUPPORT_GDI_ARB,
	        WGL_SUPPORT_OPENGL_ARB,
	        WGL_DOUBLE_BUFFER_ARB,
	        WGL_STEREO_ARB,
	        WGL_COLOR_BITS_ARB,
	        WGL_RED_BITS_ARB,
	        WGL_RED_SHIFT_ARB,
	        WGL_GREEN_BITS_ARB,
	        WGL_GREEN_SHIFT_ARB,
	        WGL_BLUE_BITS_ARB,
	        WGL_BLUE_SHIFT_ARB,
	        WGL_ALPHA_BITS_ARB,
	        WGL_ALPHA_SHIFT_ARB,
	        WGL_ACCUM_BITS_ARB,
	        WGL_ACCUM_RED_BITS_ARB,
	        WGL_ACCUM_GREEN_BITS_ARB,
	        WGL_ACCUM_BLUE_BITS_ARB,
	        WGL_ACCUM_ALPHA_BITS_ARB,
	        WGL_DEPTH_BITS_ARB,
	        WGL_STENCIL_BITS_ARB,
	        WGL_AUX_BUFFERS_ARB
		};

		template <typename T>
		T getAttribValue(const jutil::Queue<int> &values, int name) {
			for (size_t i = 0; i < ATTRIB_NAMES.size(); ++i) {
				if (ATTRIB_NAMES[i] == name) return static_cast<T>(values[i]);
			}
			return static_cast<T>(ATTRIB_NOT_FOUND);
		}

		PixelFormat generateConfig(JWIN_DISPLAY_CONTEXT hdc, const JWIN_RAW_FBCONFIG &cfg) {
			PixelFormat r;
			r.id = 0;

			jutil::Queue<int> values;
			values.reserve(ATTRIB_NAMES.size());
			values.resize(ATTRIB_NAMES.size());

			int *attrs = new int[ATTRIB_NAMES.size()];

			for (size_t i = 0; i < ATTRIB_NAMES.size(); ++i) attrs[i] = ATTRIB_NAMES[i];

			if (JWinGetPixelFormatAttribiv(hdc, cfg, 0, ATTRIB_NAMES.size(), attrs, values.getArray())) {

				r.id = (JWIN_ID)cfg;

				r.transparent = getAttribValue<GLboolean>(values, WGL_TRANSPARENT_ARB);
				r.stereoColor = getAttribValue<GLboolean>(values, WGL_STEREO_ARB);
				r.nAuxBuffers = getAttribValue<GLint>(values, WGL_AUX_BUFFERS_ARB);
				r.samples = getAttribValue<GLint>(values, WGL_SAMPLES_ARB);

				r.accumMask = createMask(jutil::Queue<uint8_t> {
					getAttribValue<uint8_t>(values, WGL_ACCUM_ALPHA_BITS_ARB),
					getAttribValue<uint8_t>(values, WGL_ACCUM_BLUE_BITS_ARB),
					getAttribValue<uint8_t>(values, WGL_ACCUM_GREEN_BITS_ARB),
					getAttribValue<uint8_t>(values, WGL_ACCUM_RED_BITS_ARB)
				});

				r.renderable = getAttribValue<GLboolean>(values, WGL_SUPPORT_OPENGL_ARB);

				r.drawableType = 0;
				r.renderType = 0;

				if (getAttribValue<GLboolean>(values, WGL_DRAW_TO_WINDOW_ARB)) r.drawableType |= JWIN_TARGET_WINDOW;
				if (getAttribValue<GLboolean>(values, WGL_DRAW_TO_BITMAP_ARB)) r.drawableType |= JWIN_TARGET_BITMAP;

				if (getAttribValue<GLboolean>(values, WGL_TYPE_RGBA_ARB)) r.renderType |= JWIN_RENDER_RGBA;
				if (getAttribValue<GLboolean>(values, WGL_TYPE_COLORINDEX_ARB)) r.renderType |= JWIN_RENDER_COLORINDEX;

				r.doubleBuffered = getAttribValue<GLboolean>(values, WGL_DOUBLE_BUFFER_ARB);

				r.rgbaMask = createMask(jutil::Queue<uint8_t> {
					getAttribValue<uint8_t>(values, WGL_ALPHA_BITS_ARB),
					getAttribValue<uint8_t>(values, WGL_BLUE_BITS_ARB),
					getAttribValue<uint8_t>(values, WGL_GREEN_BITS_ARB),
					getAttribValue<uint8_t>(values, WGL_RED_BITS_ARB)
				});

				r.dsMask = createMask(jutil::Queue<uint16_t> {
					getAttribValue<uint16_t>(values, WGL_STENCIL_BITS_ARB),
					getAttribValue<uint16_t>(values, WGL_DEPTH_BITS_ARB)
				});
			}


			delete[] attrs;

			return r;
		}

		bool validConfig(const PixelFormat &cfg) {
			return (
				cfg.id &&
				cfg.doubleBuffered && 
				cfg.rgbaMask == JWIN_RGBA && 
				cfg.renderable &&  
				(cfg.renderType & JWIN_RENDER_RGBA) && 
				(cfg.drawableType & JWIN_TARGET_WINDOW) 
			);
		}

		jutil::Queue<PixelFormat> getAllConfigs(JWIN_DISPLAY_CONTEXT hdc) {

			if (!hdc) jutil::out << "3X" << jutil::endl;

			jutil::out << "3A" << jutil::endl;

			jutil::Queue<PixelFormat> r;

			jutil::out << "3B" << jutil::endl;

			int attribNameCount = WGL_NUMBER_PIXEL_FORMATS_ARB, attribValueCount = 0;
			JWinGetPixelFormatAttribiv(hdc, 1, 0, 1, &attribNameCount, &attribValueCount);
			jutil::out << "3C" << jutil::endl;

			r.reserve(attribValueCount);

			for(int i = 0; i < attribValueCount; ++i) {
				jutil::out << "3D" << i << jutil::endl;
				PixelFormat pf = generateConfig(hdc, i + 1);
				if (validConfig(pf)) r.insert(pf);
			}

			return r;
		}

		void populateSupportedExtensions(JWIN_DISPLAY_CONTEXT hdc) {

			jutil::String supportedExtensionsStr = (jutil::String)(JWinGetExtensionsString(hdc));

			supportedExtensions = jutil::split(supportedExtensionsStr, ' ');
		}

		ContextData createContext(Handle handle, JWIN_DISPLAY_CONTEXT hdc, const ContextSettings &settings) {
			ContextData r;
			const PixelFormat *PF = &(r.settings.pixelFormat);
			PIXELFORMATDESCRIPTOR pfd = {0};

			r.displayContext = hdc;
			r.settings = settings;
			r.drawableObject = handle;

			DescribePixelFormat(hdc, PF->id, sizeof(pfd), &pfd);
			SetPixelFormat(hdc, PF->id, &pfd);

			if (supportedExtensions.find(jutil::String("WGL_ARB_create_context"))) {
				int *contextAttribs = new int[9];

				int i = 0;

				contextAttribs[i++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
				contextAttribs[i++] = settings.glVersionMajor;

				contextAttribs[i++] = WGL_CONTEXT_MINOR_VERSION_ARB;
				contextAttribs[i++] = settings.glVersionMinor;

				if (settings.forwardCompatible || settings.debug) {
					contextAttribs[i++] = WGL_CONTEXT_FLAGS_ARB;
					contextAttribs[i++] = 0;
					if (settings.forwardCompatible) contextAttribs[i - 1] |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
					if (settings.debug) contextAttribs[i - 1] |= WGL_CONTEXT_DEBUG_BIT_ARB;
				}

				contextAttribs[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
				contextAttribs[i++] = settings.profile;

				contextAttribs[i] = 0;

				r.renderContext = JWinCreateContextAttribs(hdc, NULL, contextAttribs);
			} else {
				r.renderContext = wglCreateContext(hdc);
			}

			if (r.displayContext && r.renderContext) contexts.insert(r);

			return r;
		}

		ContextID createContext(Handle drawable) {
			ContextID id = JWIN_INVALID;
			if (_wmg::windowIsRegistered(drawable)) {
				HWND win = (HWND)drawable;
				if (!fwininit) win32init(win);

		    	size_t contextCount = contexts.size();

		    	desiredContextSettings.pixelFormat = 
		    		getNearestConfig(
		    			desiredContextSettings.pixelFormat, 
		    			getAllConfigs(GetDC(win))
		    		)
		    	;

				createContext(drawable, GetDC(win), desiredContextSettings);

				if (contexts.size() > contextCount) id = contexts.size() - 1;
			}
			return id;
		}
		bool setContext(ContextID id) {
			if (id != JWIN_INVALID && id < contexts.size() && _wmg::windowIsRegistered(contexts[id].drawableObject)) {
				makeContextCurrent(&(contexts[id]));
				return true;
			} else return false;
		}

		void makeContextCurrent(ContextData *context) {
			if (wglMakeCurrent(
				context->displayContext,
				context->renderContext
			)) {
				jutil::out << "Context made current." << jutil::endl;
				currentContext = context;
			}

			if (glewInit() == GLEW_OK) jutil::out << "GLEW initialized." << jutil::endl;
		}

		int CALLBACK cbMonitorInfo(HMONITOR__* monitor, HDC__*, tagRECT *rect, LPARAM lpid) {
			size_t *id = reinterpret_cast<size_t*>(lpid);

			MONITORINFOEX info;
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(monitor, &info);

			if (!(info.dwFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {

				monitorData.monitors.insert(new Monitor(
					jutil::String(info.szDevice),
					static_cast<unsigned long>(*id),
					{static_cast<size_t>(info.rcMonitor.right - info.rcMonitor.left), static_cast<size_t>(info.rcMonitor.bottom - info.rcMonitor.top)},
					{info.rcMonitor.left, info.rcMonitor.top}
				));

				if (info.dwFlags & 1) monitorData.primaryMonitor = monitorData.monitors.last();
			}

			++(*id);

			return true;
		}

		bool createDisplay() {
			size_t id = 0;
			EnumDisplayMonitors(NULL, NULL, &cbMonitorInfo, reinterpret_cast<LPARAM>(&id));
			if (monitorData.monitors.empty()) {
				jutil::err << "JWin: Unable to find any connected monitors." << jutil::endl;
				return false;
			}
			else if (!monitorData.primaryMonitor) monitorData.primaryMonitor = monitorData.monitors.first();

			return true;
		}
		void destroyDisplay() {
			monitorData.primaryMonitor = nullptr;
			monitorData.monitors.clear();
		}
		void setVSync(Handle, bool s) {
			JWinSwapIntervalEXT((s? 1 : 0));
		}
		void swapBuffers(Handle win) {
			SwapBuffers(GetDC((HWND)win));
		}
	}
}

#endif