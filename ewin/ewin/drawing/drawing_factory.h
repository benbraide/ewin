#pragma once

#ifndef EWIN_DRAWING_FACTORY_H
#define EWIN_DRAWING_FACTORY_H

#include "../common/boolean_property.h"
#include "../common/object_property.h"

#include "drawing_type_aliases.h"

#include "drawing_factory.h"

namespace ewin::application{
	class object;
}

namespace ewin::drawing{
	class factory{
	public:
		struct create_info{};

		factory();

		virtual ~factory();

		factory(const factory &) = delete;
		
		factory &operator =(const factory &) = delete;

		common::read_only_object_value_property<application::object, factory> app;
		common::read_only_object_value_property<types::factory, factory> native;
		common::boolean_value_property<factory> created;

	protected:
		friend class application::object;

		virtual void handle_property_(void *prop, void *arg, common::property_access access);

		virtual void create_(bool create, const create_info *info);

		application::object *app_;
		types::factory *native_;
	};
}

#endif /* !EWIN_DRAWING_FACTORY_H */
