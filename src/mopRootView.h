#pragma once

#include "mopView.h"

namespace mop {
	class RootView : public View {
	public:
		void draw() override;
		bool hitTest(vec2 p) override { return true; }
	};
}