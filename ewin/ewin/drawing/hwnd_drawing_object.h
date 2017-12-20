#pragma once

#ifndef EWIN_HWND_DRAWING_OBJECT_H
#define EWIN_HWND_DRAWING_OBJECT_H

#include "drawing_object.h"

namespace ewin::window{
	class object;
}

namespace ewin::drawing{
	class hwnd_object : public typed_object<types::hwnd_render_target>{
	public:
		typedef typed_object<types::hwnd_render_target> base_type;

		struct create_info : base_type::create_info{
			create_info(drawing::factory &factory)
				: factory(factory){}

			std::reference_wrapper<drawing::factory> factory;
		};

		hwnd_object();

		explicit hwnd_object(const create_info &info);

		virtual ~hwnd_object();

		common::object_value_property<drawing::factory, hwnd_object> factory;
		common::object_value_property<window::object, hwnd_object> target;
		common::write_only_boolean_value_property<hwnd_object> lock_target;

	protected:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual void create_(bool create, const base_type::create_info *info) override;

		virtual void recreate_() override;

		virtual void low_level_create_(const create_info &info);

		drawing::factory *factory_;
		window::object *target_;
		std::size_t size_event_id_;
		std::size_t destroy_event_id_;
		bool target_locked_;
	};
}

#endif /* !EWIN_HWND_DRAWING_OBJECT_H */
