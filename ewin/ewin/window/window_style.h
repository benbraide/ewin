#pragma once

#ifndef EWIN_WINDOW_STYLE_H
#define EWIN_WINDOW_STYLE_H

#include "../common/type_aliases.h"
#include "../common/state_property.h"

#include "window_style_helper.h"

namespace ewin::window{
	class wnd_style : public wnd_style_helper{
	public:
		explicit wnd_style(object &target);

		wnd_style(const wnd_style &) = delete;

		wnd_style &operator =(const wnd_style &) = delete;

		common::state_value_property<common::types::uint, wnd_style> value;
		common::state_value_property<common::types::uint, wnd_style> extended_value;

	private:
		friend class object;

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);
	};
}

#endif /* !EWIN_WINDOW_STYLE_H */
