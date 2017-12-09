#pragma once

#ifndef EWIN_WINDOW_STATE_H
#define EWIN_WINDOW_STATE_H

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"

#include "window_style_helper.h"

namespace ewin::window{
	class wnd_state : public wnd_style_helper{
	public:
		explicit wnd_state(object &target);

		wnd_state(const wnd_state &) = delete;

		wnd_state &operator =(const wnd_state &) = delete;

		common::boolean_value_property<wnd_state> enabled;
		common::boolean_value_property<wnd_state> top_most;

		common::boolean_value_property<wnd_state> tab_stop;
		common::boolean_value_property<wnd_state> file_drop;
		common::boolean_value_property<wnd_state> mouse_activate;

		common::boolean_value_property<wnd_state> clip_children;
		common::boolean_value_property<wnd_state> clip_siblings;

	private:
		friend class object;

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);
	};
}

#endif /* !EWIN_WINDOW_STATE_H */
