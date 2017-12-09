#pragma once

#ifndef ELANG_WINDOW_FRAME_H
#define ELANG_WINDOW_FRAME_H

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"

#include "window_style_helper.h"

namespace ewin::window{
	class wnd_frame : public wnd_style_helper{
	public:
		explicit wnd_frame(object &target);

		wnd_frame(const wnd_frame &) = delete;
		
		wnd_frame &operator =(const wnd_frame &) = delete;

		common::boolean_value_property<wnd_frame> visible;

		common::boolean_value_property<wnd_frame> show_caption;
		common::boolean_value_property<wnd_frame> show_system_menu;
		common::boolean_value_property<wnd_frame> show_border;

		common::boolean_value_property<wnd_frame> enable_minimize_button;
		common::boolean_value_property<wnd_frame> enable_maximize_button;
		common::boolean_value_property<wnd_frame> enable_close_button;

		common::boolean_value_property<wnd_frame> thick;
		common::boolean_value_property<wnd_frame> static_edge;
		common::boolean_value_property<wnd_frame> window_edge;
		common::boolean_value_property<wnd_frame> sunken_edge;
		common::boolean_value_property<wnd_frame> dialog_modal;

	private:
		friend class object;

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);
	};
}

#endif /* !ELANG_WINDOW_FRAME_H */
