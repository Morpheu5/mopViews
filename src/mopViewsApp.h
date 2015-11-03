#pragma once

namespace mop {
	class mopViewsApp {
	public:
		virtual vec2 screenToWorld(const vec2&) = 0;
	};
}