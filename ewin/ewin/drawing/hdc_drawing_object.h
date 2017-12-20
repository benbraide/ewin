#pragma once

#ifndef EWIN_HDC_DRAWING_OBJECT_H
#define EWIN_HDC_DRAWING_OBJECT_H

#include "drawing_object.h"

namespace ewin::drawing{
	class hdc_object : public typed_object<types::hdc_render_target>{
	public:
		typedef typed_object<types::hdc_render_target> base_type;

		struct create_info : base_type::create_info{
			create_info(drawing::factory &factory)
				: factory(factory){}

			std::reference_wrapper<drawing::factory> factory;
		};

		hdc_object();

		explicit hdc_object(const create_info &info);

		virtual ~hdc_object();

		common::object_value_property<drawing::factory, hdc_object> factory;
		common::value_property<common::types::hdc, hdc_object> target;
		common::point_value_property<common::types::point, hdc_object> offset;
		common::write_only_boolean_value_property<hdc_object> lock_target;

	protected:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual void create_(bool create, const base_type::create_info *info) override;

		virtual void recreate_() override;

		virtual void low_level_create_(const create_info &info);

		drawing::factory *factory_;
		common::types::hdc target_;
		common::types::point offset_;
		bool target_locked_;
	};
}

#endif /* !EWIN_HDC_DRAWING_OBJECT_H */
