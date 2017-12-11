#pragma once

#ifndef EWIN_EXTERNAL_WINDOW_H
#define EWIN_EXTERNAL_WINDOW_H

#include "window_object.h"

namespace ewin::window{
	class external : public object{
	public:
		explicit external(common::types::hwnd value){
			handle_ = value;
			auto_destroy_ = false;//Prevent destruction
		}
	};
}

#endif /* !EWIN_EXTERNAL_WINDOW_H */
