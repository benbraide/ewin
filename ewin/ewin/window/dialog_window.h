#pragma once

#ifndef EWIN_DIALOG_WINDOW_H
#define EWIN_DIALOG_WINDOW_H

#include "window_object.h"

namespace ewin::window{
	class dialog : public object{
	public:
		dialog(){
			procedure_ = ::DefDlgProcW;
		}

		virtual ~dialog(){
			destruct_();
		}
	};
}

#endif /* !EWIN_DIALOG_WINDOW_H */
