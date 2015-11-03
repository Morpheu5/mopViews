#pragma once

#include "mopView.h"

using namespace std;

namespace mop {
	class ViewController {
	public:
		virtual shared_ptr<View> getView() = 0;
	};
}