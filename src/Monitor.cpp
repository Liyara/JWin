#include <JWin/Monitor.h>

namespace jwin {
	Monitor::Monitor(const jutil::String &n, unsigned long i, const Dimensions &s, const Position &p) : name(n), id(i), geometry(p, s) {
	}
	const jutil::String &Monitor::getName() const {
		return name;
	}
	const unsigned long &Monitor::getID() const {
		return id;
	}
	const Dimensions &Monitor::getSize() const {
		return geometry.size;
	}
	const Position &Monitor::getPosition() const {
		return geometry.position;
	}
}