#ifndef JWIN_SCREEN_H
#define JWIN_SCREEN__H

#include <JWin/types.h>
#include <JUtil/Core/String.h>

namespace jwin {

	class Monitor {
	public:
		Monitor(const jutil::String&, unsigned long, const Dimensions&, const Position& = {0, 0});
		const jutil::String &getName() const;
		const unsigned long &getID() const;
		const Dimensions &getSize() const;
		const Position &getPosition() const;
	private:
		jutil::String name;
		unsigned long id;
		Geometry geometry;
	};
}

#endif