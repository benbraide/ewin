#pragma once

#ifndef EWIN_MAIN_APPLICATION_H
#define EWIN_MAIN_APPLICATION_H

#include "../window/window_class.h"

#include "application_manager.h"

namespace ewin::application{
	class main_app : public object{
	public:
		main_app();

		~main_app();

		window::wnd_class general_window_class;
		window::wnd_class dialog_window_class;
	};
}

#endif /* !EWIN_MAIN_APPLICATION_H */
