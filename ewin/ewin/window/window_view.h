#pragma once

#ifndef EWIN_WINDOW_VIEW_H
#define EWIN_WINDOW_VIEW_H

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"

#include "window_style_helper.h"

namespace ewin::window{
	class wnd_view : public wnd_style_helper{
	public:
		explicit wnd_view(object &target);

		wnd_view(const wnd_view &) = delete;
		
		wnd_view &operator =(const wnd_view &) = delete;

		common::boolean_value_property<wnd_view> visible;
		common::write_only_value_property<common::types::uint, wnd_view> show;
		common::write_only_value_property<common::types::uint, wnd_view> show_async;

		common::boolean_value_property<wnd_view> maximized;
		common::boolean_value_property<wnd_view> minimized;

		common::boolean_value_property<wnd_view> transparent;

	private:
		friend class object;

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		common::types::uint show_type_;
	};
}

#endif /* !EWIN_WINDOW_VIEW_H */
