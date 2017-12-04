#pragma once

#ifndef EWIN_APPLICATION_OBJECT_H
#define EWIN_APPLICATION_OBJECT_H

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"

namespace ewin::window{
	class object;
}

namespace ewin::application{
	class object{
	public:
		typedef common::error_type error_type;
		typedef window::object window_type;

		struct error_info{
			window_type *owner;
			error_type value;
		};

		object();

		common::value_property<error_info, object> last_error;
		common::read_only_value_property<window_type *, object> last_error_owner;
		common::value_property<error_type, object> last_error_value;

		static object *main;
		static thread_local object *current;

	private:
		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		error_info last_error_info_;
	};
}

#endif /* !EWIN_APPLICATION_OBJECT_H */
