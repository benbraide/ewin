#pragma once

#ifndef EWIN_WINDOW_ATTRIBUTE_H
#define EWIN_WINDOW_ATTRIBUTE_H

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/object_property.h"

namespace ewin::window{
	class object;

	class wnd_attribute{
	public:
		explicit wnd_attribute(object &target);

		wnd_attribute(const wnd_attribute &) = delete;

		wnd_attribute &operator =(const wnd_attribute &) = delete;

		common::read_only_object_value_property<object, wnd_attribute> owner;

		common::read_only_boolean_value_property<wnd_attribute> is_windowless;
		common::read_only_boolean_value_property<wnd_attribute> is_group;

		common::read_only_boolean_value_property<wnd_attribute> is_dialog;
		common::read_only_boolean_value_property<wnd_attribute> is_modal;

		common::read_only_boolean_value_property<wnd_attribute> is_message_only;
		common::read_only_boolean_value_property<wnd_attribute> is_control;

	private:
		friend class object;

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		object *target_;
	};
}

#endif /* !EWIN_WINDOW_ATTRIBUTE_H */
