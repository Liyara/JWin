#include <JWin/jwin_x11.h>
#ifdef JUTIL_LINUX

namespace jwin {
	namespace display_manager {

		DisplayData displayData;

		template <typename T>
		T getAtrrib(JWIN_DISPLAY_CONTEXT display, int attr, const JWIN_RAW_FBCONFIG &cfg) {
			int r;
			glXGetFBConfigAttrib(display, cfg, attr, &r);
			return static_cast<T>(r);
		}

		PixelFormat generateConfig(JWIN_DISPLAY_CONTEXT display, const JWIN_RAW_FBCONFIG &cfg) {
			PixelFormat r;
			
			r.platformConfig.glxConfig = cfg;
			r.platformConfig.visualInfo = glXGetVisualFromFBConfig(display, cfg);

			if (r.platformConfig.visualInfo) r.platformConfig.format = XRenderFindVisualFormat(display, r.platformConfig.visualInfo->visual);
			if (r.platformConfig.format) r.transparent = (r.platformConfig.format->direct.alphaMask > 0? 1 : 0);
			else r.transparent = 0;

			r.id 							= getAtrrib<XID>(display, GLX_FBCONFIG_ID, cfg);
			r.platformConfig.vid 			= getAtrrib<XID>(display, GLX_VISUAL_ID, cfg);
			r.platformConfig.visualType 	= getAtrrib<GLint>(display, GLX_X_VISUAL_TYPE, cfg);
			r.platformConfig.bufferLevel 	= getAtrrib<GLint>(display, GLX_LEVEL, cfg);

			r.stereoColor 	= getAtrrib<GLboolean>(display, GLX_STEREO, cfg);
			r.nAuxBuffers 	= getAtrrib<GLint>(display, GLX_AUX_BUFFERS, cfg);
			r.samples 		= getAtrrib<GLint>(display, GLX_SAMPLES, cfg);

			r.accumMask = createMask(jutil::Queue<uint8_t> {
				getAtrrib<uint8_t>(display, GLX_ACCUM_ALPHA_SIZE, cfg),
				getAtrrib<uint8_t>(display, GLX_ACCUM_BLUE_SIZE, cfg),
				getAtrrib<uint8_t>(display, GLX_ACCUM_GREEN_SIZE, cfg),
				getAtrrib<uint8_t>(display, GLX_ACCUM_RED_SIZE, cfg),
			});

			r.renderable 		= getAtrrib<GLboolean>(display, GLX_X_RENDERABLE, cfg);
			r.doubleBuffered 	= getAtrrib<GLboolean>(display, GLX_DOUBLEBUFFER, cfg);
			r.drawableType 		= getAtrrib<GLint>(display, GLX_DRAWABLE_TYPE, cfg);
			r.renderType 		= getAtrrib<GLint>(display, GLX_RENDER_TYPE, cfg);

			r.rgbaMask = createMask(jutil::Queue<uint8_t> {
				getAtrrib<uint8_t>(display, GLX_ALPHA_SIZE, cfg),
				getAtrrib<uint8_t>(display, GLX_BLUE_SIZE, cfg),
				getAtrrib<uint8_t>(display, GLX_GREEN_SIZE, cfg),
				getAtrrib<uint8_t>(display, GLX_RED_SIZE, cfg),
			});

			r.dsMask = createMask(jutil::Queue<uint16_t> {
				getAtrrib<uint16_t>(display, GLX_STENCIL_SIZE, cfg),
				getAtrrib<uint16_t>(display, GLX_DEPTH_SIZE, cfg),
			});

			return r;
		}


		void printConfig(const PixelFormat &cfg) {
			jutil::out << "RGBA: " << cfg.rgbaMask << jutil::endl;
			jutil::out << "DS: " << cfg.dsMask << jutil::endl;
			jutil::out << "accum: " << cfg.accumMask << jutil::endl;
			jutil::out << "transparent: " << (bool)cfg.transparent << jutil::endl;
			jutil::out << "samples: " << cfg.samples << jutil::endl;
			jutil::out << jutil::endl;
		}

		bool validConfig(const PixelFormat &cfg) {
			if (!cfg.id) return true; //idealConfig passed, ignore.
			return (
				cfg.platformConfig.visualInfo &&
				cfg.doubleBuffered && 
				cfg.rgbaMask == (JWIN_RGBA) && 
				cfg.renderable &&  
				(cfg.renderType & JWIN_RENDER_RGBA) && 
				cfg.platformConfig.visualType == GLX_TRUE_COLOR &&
				(cfg.drawableType & JWIN_TARGET_WINDOW) 
			);
		}

		PixelFormatList getAllConfigs(JWIN_DISPLAY_CONTEXT display) {
			jutil::out << "Getting all compatible configurations..." << jutil::endl;
			JWIN_RAW_FBCONFIG *fbc;
			int fbcount;
			PixelFormatList r;
			fbc = glXGetFBConfigs(display, displayData.currentScreenID, &fbcount);

			r.reserve(fbcount);

			JWIN_RAW_FBCONFIG bestConfig = {0};

			for (size_t i = 0; i < fbcount; ++i) {
				PixelFormat cfg = generateConfig(display, fbc[i]);
				
				if (validConfig(cfg)) {
					r.insert(cfg);
					jutil::out << "Got config " << cfg.id << jutil::endl;
					printConfig(cfg);
				}
			}

			XFree(fbc);

			return r;
		}

		ContextData createContext(Handle handle, JWIN_DISPLAY_CONTEXT display, const ContextSettings &cfg) {
			jutil::out << "Creating context..." << jutil::endl;
			ContextData r;

			r.settings = cfg;
			r.displayContext = display;
			r.drawableObject = handle;

			if (supportedExtensions.find(jutil::String("GLX_ARB_create_context"))) {
				int *contextAttribs = new int[11];

				int i = 0;

				contextAttribs[i++] = GLX_CONTEXT_MAJOR_VERSION_ARB;
				contextAttribs[i++] = cfg.glVersionMajor;

				contextAttribs[i++] = GLX_CONTEXT_MINOR_VERSION_ARB;
				contextAttribs[i++] = cfg.glVersionMinor;

				if (cfg.forwardCompatible || cfg.debug) {

					contextAttribs[i++] = GLX_CONTEXT_FLAGS_ARB;
					contextAttribs[i++] = 0;
					if (cfg.forwardCompatible) contextAttribs[i - 1] |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
					if (cfg.debug) contextAttribs[i - 1] |= GLX_CONTEXT_DEBUG_BIT_ARB;
				}

				contextAttribs[i++] = GLX_CONTEXT_PROFILE_MASK_ARB;
				contextAttribs[i++] = cfg.profile;

				contextAttribs[i++] = GLX_RENDER_TYPE;
				contextAttribs[i++] = GLX_RGBA_TYPE;

				contextAttribs[i] = None;

				r.renderContext = JWinCreateContextAttribs(display, cfg.pixelFormat.platformConfig.glxConfig, NULL, True, contextAttribs);

				delete[] contextAttribs;
			} else r.renderContext = glXCreateNewContext(display, cfg.pixelFormat.platformConfig.glxConfig, GLX_RGBA_TYPE, NULL, True);

			if (r.displayContext && r.renderContext) contexts.insert(r);

			return r;
		}

		ContextID createContext(Handle drawable) {
			ContextID id = JWIN_INVALID;
			if (_wmg::windowIsRegistered(drawable)) {
				displayData.contextData->drawableObject = drawable;
				ContextID cid = 0;
				for (auto &i: contexts) {
					if (displayData.contextData == &i) {
						id = cid;
					}
					++cid;
				}
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
			if (glXMakeCurrent(
				context->displayContext,
				*((XWindow*)context->drawableObject),
				context->renderContext
			)) {
				currentContext = context;
			}

			if (glewInit() == GLEW_OK) jutil::out << "GLEW initialized." << jutil::endl;
		}

		void populateSupportedExtensions(JWIN_DISPLAY_CONTEXT display) {
			supportedExtensions = jutil::split(
				jutil::String(
					glXQueryExtensionsString(
						display, 
						displayData.currentScreenID
					)
				), char(' ')
			);
		}

		bool createDisplay() {

			jutil::out << "Creating display..." << jutil::endl;

			XRRScreenResources *screenR;
			XRRCrtcInfo *crtcInfo;
			XRROutputInfo *info;

			#ifdef JWIN_SET_DISPLAY_ADAPTER
				displayData.display = XOpenDisplay(JWIN_SET_DISPLAY_ADAPTER);
			#else
				displayData.display = XOpenDisplay(NULL);
			#endif

			if (displayData.display) {

				jutil::out << "Display opened." << jutil::endl;

				displayData.currentScreenID = XDefaultScreen(displayData.display);
			   	displayData.rootWindow = XDefaultRootWindow(displayData.display);

			   	populateSupportedExtensions(displayData.display);

				jutil::out << "Supported extensions:" << supportedExtensions << jutil::endl;

				desiredContextSettings.pixelFormat = getNearestConfig(desiredContextSettings.pixelFormat, getAllConfigs(displayData.display));
				createContext(nullptr, displayData.display, desiredContextSettings);
				if (!contexts.empty()) displayData.contextData = &(contexts[0]);
				else displayData.contextData = nullptr;

				XSync(displayData.display, False);

				if (!displayData.contextData) {
					jutil::out << "Failed to create context!" << jutil::endl;
				} else {
					jutil::out << "Context created successfuly." << jutil::endl;
				}

			   	screenR = XRRGetScreenResources(displayData.display, displayData.rootWindow);
			   	RROutput prim = XRRGetOutputPrimary(displayData.display, displayData.rootWindow);

			   	windowState 			= XInternAtom(displayData.display, "_NET_WM_STATE", True);	 
				windowMaximizeX 		= XInternAtom(displayData.display, "_NET_WM_STATE_MAXIMIZED_VERT", True);
				windowMaximizeY 		= XInternAtom(displayData.display, "_NET_WM_STATE_MAXIMIZED_HORZ", True);
				windowMinimize 			= XInternAtom(displayData.display, "_NET_WM_STATE_HIDDEN", True);
				windowIgnoreTaskbar		= XInternAtom(displayData.display, "_NET_WM_STATE_SKIP_TASKBAR", True);
				windowIgnorePager 		= XInternAtom(displayData.display, "_NET_WM_STATE_SKIP_PAGER", True);
				windowFullscreen 		= XInternAtom(displayData.display, "_NET_WM_STATE_FULLSCREEN", True);
				windowAlert 			= XInternAtom(displayData.display, "_NET_WM_STATE_DEMANDS_ATTENTION", True);
				windowOnTop 			= XInternAtom(displayData.display, "_NET_WM_STATE_ABOVE", True);
				windowOnBottom 			= XInternAtom(displayData.display, "_NET_WM_STATE_BELOW", True);

				windowType 				= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE", True);
				windowTypeDesktop		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_DESKTOP", True);
				windowTypeDock			= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_DOCK", True);
				windowTypeToolbar		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_TOOLBAR", True);
				windowTypeMenu			= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_MENU", True);
				windowTypeUtility		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_UTILITY", True);
				windowTypeSplash		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_SPLASH", True);
				windowTypeDialog		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_DIALOG", True);
				windowTypeNormal		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_NORMAL", True);
				windowTypeUndecorated   = XInternAtom(displayData.display, "_MOTIF_WM_HINTS", True);

				windowPermissions		= XInternAtom(displayData.display, "_NET_WM_ALLOWED_ACTIONS", True);
				windowAllowMove			= XInternAtom(displayData.display, "_NET_WM_ACTION_MOVE", True);
				windowAllowResize		= XInternAtom(displayData.display, "_NET_WM_ACTION_RESIZE", True);
				windowAllowMinimize		= XInternAtom(displayData.display, "_NET_WM_ACTION_MINIMIZE", True);
				windowAllowMaximizeX	= XInternAtom(displayData.display, "_NET_WM_ACTION_MAXIMIZE_HORZ", True);
				windowAllowMaximizeY	= XInternAtom(displayData.display, "_NET_WM_ACTION_MAXIMIZE_VERT", True);
				windowAllowFullscreen	= XInternAtom(displayData.display, "_NET_WM_ACTION_FULLSCREEN", True);

				windowDestroy			= XInternAtom(displayData.display, "WM_DELETE_WINDOW", True);
				windowOpacity			= XInternAtom(displayData.display, "_NET_WM_WINDOW_OPACITY", True);

			   	for (size_t i = 0; i < screenR->ncrtc; ++i) {
					for (size_t j = 0; j < screenR->noutput; ++j) {
						if (screenR) {
							if (screenR->crtcs[i] != None && screenR->outputs[j] != None) {
								info = XRRGetOutputInfo(displayData.display, screenR, screenR->outputs[j]);
								if (info) {

									if (info->crtc != None && info->crtc == screenR->crtcs[i]) {
										bool dupe = false;
										for (auto &k: monitorData.monitors) {
											if (k->getID() == screenR->outputs[j]) {
												dupe = true;
												break;
											}
										}

										if (!dupe) {
											crtcInfo = XRRGetCrtcInfo(displayData.display, screenR, screenR->crtcs[i]);
											if (crtcInfo) {

												monitorData.monitors.insert(new Monitor(
													jutil::String(info->name),
													screenR->outputs[j],
													{static_cast<size_t>(crtcInfo->width), static_cast<size_t>(crtcInfo->height)},
													{crtcInfo->x, crtcInfo->y}
												));

												if (prim == screenR->outputs[j]) monitorData.primaryMonitor = monitorData.monitors.last();

												XRRFreeCrtcInfo(crtcInfo);
					   							crtcInfo = nullptr;

											} else jutil::err << "JWin: failed to retrieve info about CRTC " << screenR->crtcs[i] << "." << jutil::endl;
										}

									}

									XRRFreeOutputInfo(info);
			   						info = nullptr;

								} else jutil::err << "JWin: failed to retrieve info about output " << screenR->outputs[j] << "." << jutil::endl;
							}

						} else jutil::err << "JWin: failed to retrieve screen info from XServer." << jutil::endl;
					}
			   	}

			   	if (monitorData.monitors.empty()) {
			   		jutil::err << "JWin: Unable to find any connected monitors." << jutil::endl;
			   		return false;
			   	} else if (!monitorData.primaryMonitor) monitorData.primaryMonitor = monitorData.monitors.first();

			} else {
				jutil::err << "JWin: Invalid display adapter." << jutil::endl;
				return false;
			}

			XRRFreeScreenResources(screenR);
			screenR = nullptr;

			return true;
		}

		void setVSync(Handle handle, bool s) {
			int interval = (s? 1 : 0);
			if (supportedExtensions.find(jutil::String("GLX_EXT_swap_control"))) {
				JWinSwapIntervalEXT(displayData.display, *((XWindow*)handle), interval);
			} else if (supportedExtensions.find(jutil::String("GLX_MESA_swap_control"))) {
				JWinSwapIntervalMESA(interval);
			} else if (supportedExtensions.find(jutil::String("GLX_SGI_swap_control"))) {
				JWinSwapIntervalSGI(interval);
			}
		}

		void swapBuffers(Handle handle) {
			if (_wmg::windowIsRegistered(handle)) glXSwapBuffers(displayData.display, *((XWindow*)handle));
		}

		void destroyDisplay() {
			if (displayData.display) XCloseDisplay(displayData.display);
			displayData.display = nullptr;
			for (auto &i: monitorData.monitors) delete i;
			monitorData.monitors.clear();
			monitorData.primaryMonitor = nullptr;
		}
	}
}

#endif