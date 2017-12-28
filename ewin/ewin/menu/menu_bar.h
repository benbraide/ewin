#pragma once

#ifndef EWIN_MENU_BAR_H
#define EWIN_MENU_BAR_H

#include "menu_container.h"

namespace ewin::window{
	class object;
}

namespace ewin::menu{
	class bar : public container{
	public:
		bar();

		bar(const create_info &info);

		virtual ~bar();

		common::object_value_property<window::object, bar> owner;

	protected:
		void bind_properties_();

		virtual void create_(bool create, const create_info *info) override;

		virtual void child_removed_(object &value, std::size_t index) override;

		virtual void child_added_(object &value, std::size_t index) override;

		virtual void set_owner_(window::object *value);

		window::object *owner_;
		std::size_t event_id_;
	};
}

#endif /* !EWIN_MENU_BAR_H */
