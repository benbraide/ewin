#pragma once

#ifndef EWIN_SOLID_COLOR_DRAWING_BRUSH_H
#define EWIN_SOLID_COLOR_DRAWING_BRUSH_H

#include "drawing_brush.h"

namespace ewin::drawing{
	class solid_color_brush : public typed_brush<types::solid_color_brush>{
	public:
		typedef typed_brush<types::solid_color_brush> base_type;

		struct create_info : base_type::create_info{
			create_info(object &drawer, const types::color &color, float opacity = 1.0f, const types::matrix_3x2 &transform = d2d1::IdentityMatrix())
				: drawer(drawer), color(color), opacity(opacity), transform(transform){}

			create_info(object &drawer, const d2d1::ColorF &color, float opacity = 1.0f, const types::matrix_3x2 &transform = d2d1::IdentityMatrix())
				: drawer(drawer), color(color), opacity(opacity), transform(transform){}

			std::reference_wrapper<object> drawer;
			types::color color;
			float opacity;
			types::matrix_3x2 transform;
		};

		solid_color_brush();

		solid_color_brush(object &drawer, const types::color &color);

		virtual ~solid_color_brush();

		common::object_value_property<object, solid_color_brush> drawer;
		common::value_property<types::color, solid_color_brush> color;

	protected:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual void create_(bool create, const base_type::create_info *info) override;

		virtual void low_level_create_(const create_info &info);

		object *drawer_;
		types::color color_;
	};
}

#endif /* !EWIN_SOLID_COLOR_DRAWING_BRUSH_H */
