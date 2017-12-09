#pragma once

#ifndef EWIN_APPLICATION_MANAGER_H
#define EWIN_APPLICATION_MANAGER_H

#include <memory>

#include "application_object.h"

namespace ewin::application{
	class manager_initializer;

	class manager{
	public:
		static common::read_only_object_value_property<object, manager> main;
		static common::read_only_object_value_property<object, manager> current;

	private:
		friend class manager_initializer;

		static void bind_properties_();

		static void handle_property_(void *prop, void *arg, common::property_access access);

		static object *main_;
		static thread_local std::shared_ptr<object> current_;
	};

	class manager_initializer{
	public:
		manager_initializer();
	} manager_initializer_;
}

#endif /* !EWIN_APPLICATION_MANAGER_H */
