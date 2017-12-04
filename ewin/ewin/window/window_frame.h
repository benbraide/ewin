#pragma once

#ifndef ELANG_WINDOW_FRAME_H
#define ELANG_WINDOW_FRAME_H

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"

namespace ewin::window{
	class object;

	class wnd_frame{
	public:
		explicit wnd_frame(object &target);

		common::read_only_value_property<object *, wnd_frame> owner;
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
		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		common::types::ptr styles_() const;

		common::types::ptr extended_styles_() const;

		object *target_;
	};
}

#endif /* !ELANG_WINDOW_FRAME_H */
