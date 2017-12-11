#pragma once

#ifndef EWIN_APPLICATION_MANAGER_H
#define EWIN_APPLICATION_MANAGER_H

#include <memory>

#include "../window/window_class.h"

#include "application_object.h"

namespace ewin::application{
	class manager_initializer;

	class manager{
	public:
		typedef std::reference_wrapper<object> object_ref_type;
		typedef std::list<object_ref_type> object_list_type;

		typedef object_list_type::iterator object_list_iterator_type;
		typedef object_list_type::const_iterator object_list_const_iterator_type;

		static common::read_only_object_value_property<object, manager> main;
		static common::read_only_object_value_property<object, manager> current;

		static common::iterator_only_list_value_property<object, object_list_iterator_type, object_list_const_iterator_type, manager> application_list;

	private:
		friend class object;
		friend class manager_initializer;

		static void bind_properties_();

		static void handle_property_(void *prop, void *arg, common::property_access access);

		static object *main_;
		static thread_local std::shared_ptr<object> current_;

		static object_list_type application_list_;
		static object_list_type temp_application_list_;
	};

	class manager_initializer{
	public:
		manager_initializer();

		~manager_initializer();

		static window::wnd_class general_window_class;
		static window::wnd_class dialog_window_class;

	private:
		bool initialized_;
		static const manager_initializer manager_initializer_;
	};
}

#endif /* !EWIN_APPLICATION_MANAGER_H */
