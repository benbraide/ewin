#pragma once

#ifndef EWIN_WINDOW_STYLE_HELPER_H
#define EWIN_WINDOW_STYLE_HELPER_H

#include "../common/type_aliases.h"
#include "../common/object_property.h"

namespace ewin::window{
	class object;

	class wnd_style_helper{
	public:
		explicit wnd_style_helper(object &target);

		common::read_only_object_value_property<object, wnd_style_helper> owner;

	protected:
		friend class object;

		void reset_(common::types::uint value);

		void reset_extended_(common::types::uint value);

		void set_(common::types::uint value);

		void set_extended_(common::types::uint value);

		void remove_(common::types::uint value);

		void remove_extended_(common::types::uint value);

		void set_or_remove_(common::types::uint value, bool set);

		void set_or_remove_extended_(common::types::uint value, bool set);

		common::types::uint get_() const;

		common::types::uint get_extended_() const;

		object *target_;
		common::types::uint cached_value_;
		common::types::uint cached_extended_value_;
	};
}


#endif /* !EWIN_WINDOW_STYLE_HELPER_H */
