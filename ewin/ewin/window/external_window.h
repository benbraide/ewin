#pragma once

#ifndef EWIN_EXTERNAL_WINDOW_H
#define EWIN_EXTERNAL_WINDOW_H

#include "window_object.h"

namespace ewin::window{
	class external : public object{
	public:
		explicit external(common::types::hwnd value);
	};
}

#endif /* !EWIN_EXTERNAL_WINDOW_H */
