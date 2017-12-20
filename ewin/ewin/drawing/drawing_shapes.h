#pragma once

#ifndef EWIN_DRAWING_SHAPES_H
#define EWIN_DRAWING_SHAPES_H

#include "../common/numeric_property.h"

#include "com_native.h"
#include "drawing_brush.h"

namespace ewin::drawing{
	class stroke_style : public com_native<types::stroke_style>{
	public:
		typedef com_native<types::stroke_style> base_type;

		stroke_style();

		common::object_value_property<drawing::factory, stroke_style> factory;

		common::value_property<types::cap_style, stroke_style> start_cap;
		common::value_property<types::cap_style, stroke_style> end_cap;
		common::value_property<types::cap_style, stroke_style> dash_cap;

		common::value_property<types::line_join, stroke_style> line_join;
		common::numeric_value_property<float, stroke_style> miter_limit;

		common::value_property<types::dash_style, stroke_style> dash_style;
		common::numeric_value_property<float, stroke_style> dash_offset;

		common::value_property<float *, stroke_style> dashes;
		common::numeric_value_property<unsigned int, stroke_style> dash_count;

	protected:
		virtual void create_(bool create, const create_info *info) override;

		drawing::factory *factory_;
		types::stroke_style_properties cache_;
		float *dashes_;
		unsigned int dash_count_;
	};

	class stroke{
	public:
		stroke();

		common::numeric_value_property<float, stroke> width;
		common::read_only_object_value_property<stroke_style, stroke> style;

	private:
		float width_;
		stroke_style style_;
	};

	class shape{
	public:
		shape();

		virtual ~shape();

		common::read_only_object_value_property<drawing::stroke, shape> stroke;
		common::object_value_property<drawing::brush, shape> brush;
		common::object_value_property<drawing::object, shape> drawer;

		common::write_only_boolean_value_property<shape> draw;
		common::write_only_boolean_value_property<shape> fill;

	protected:
		void handle_property_(void *prop, void *arg, common::property_access access);

		virtual void draw_();

		virtual void fill_();

		drawing::stroke stroke_;
		drawing::brush *brush_;
		drawing::object *drawer_;
	};

	class rectangle_shape : public shape{
	public:
		rectangle_shape();

		virtual ~rectangle_shape();

		common::point_value_property<types::point, rectangle_shape> position;
		common::size_value_property<types::size, rectangle_shape> size;

	protected:
		virtual void draw_() override;

		virtual void fill_() override;

		types::point position_;
		types::size size_;
	};
}

#endif /* !EWIN_DRAWING_SHAPES_H */
