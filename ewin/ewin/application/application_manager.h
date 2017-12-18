#pragma once

#ifndef EWIN_APPLICATION_MANAGER_H
#define EWIN_APPLICATION_MANAGER_H

#include <memory>

#include "application_object.h"

namespace ewin::application{
	class main_app;

	class manager{
	public:
		typedef std::reference_wrapper<object> object_ref_type;
		typedef std::list<object_ref_type> object_list_type;

		typedef object_list_type::iterator object_list_iterator_type;
		typedef object_list_type::const_iterator object_list_const_iterator_type;

		static common::read_only_object_value_property<main_app, manager> main;
		static common::read_only_object_value_property<object, manager> current;
		static common::iterator_only_list_value_property<object, object_list_iterator_type, object_list_const_iterator_type, manager> application_list;

	private:
		friend class object;
		friend class main_app;

		static void bind_properties_();

		static void handle_property_(void *prop, void *arg, common::property_access access);

		static main_app *main_;
		static thread_local object *current_;

		static object_list_type application_list_;
		static object_list_type temp_application_list_;
	};
}

#endif /* !EWIN_APPLICATION_MANAGER_H */
