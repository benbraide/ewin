#include "drawing_shapes.h"

ewin::drawing::stroke_style::stroke_style()
	: factory_(nullptr), dashes_(nullptr), dash_count_(0){
	factory.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		if (access == common::property_access::read)
			*static_cast<drawing::factory **>(arg) = factory_;
		else if (access == common::property_access::write && native_ == nullptr)
			factory_ = static_cast<drawing::factory *>(arg);
	});

	cache_ = d2d1::StrokeStyleProperties();

	start_cap.initialize_(&cache_.startCap, nullptr);
	end_cap.initialize_(&cache_.endCap, nullptr);
	dash_cap.initialize_(&cache_.dashCap, nullptr);

	line_join.initialize_(&cache_.lineJoin, nullptr);
	miter_limit.initialize_(&cache_.miterLimit, nullptr);

	dash_style.initialize_(&cache_.dashStyle, nullptr);
	dash_offset.initialize_(&cache_.dashOffset, nullptr);

	dashes.initialize_(&dashes_, nullptr);
	dash_count.initialize_(&dash_count_, nullptr);
}

void ewin::drawing::stroke_style::create_(bool create, const create_info *info){
	if (create && native_ == nullptr){
		if (info == nullptr && factory_ != nullptr){
			factory_->native->CreateStrokeStyle(
				&cache_,
				dashes_,
				dash_count_,
				&native_
			);
		}
	}
	else//Forward
		base_type::create_(create, info);
}

ewin::drawing::stroke::stroke()
	: width_(1.0f){
	width.initialize_(&width_, nullptr);
	style.initialize_(&style_, nullptr);
}

ewin::drawing::shape::shape()
	: brush_(nullptr), drawer_(nullptr){
	auto handler = EWIN_PROP_HANDLER(shape);

	stroke.initialize_(&stroke_, nullptr);
	brush.initialize_(nullptr, handler);
	drawer.initialize_(nullptr, handler);

	draw.initialize_(nullptr, handler);
	fill.initialize_(nullptr, handler);
}

ewin::drawing::shape::~shape() = default;

void ewin::drawing::shape::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &brush){
		if (access == common::property_access::read)
			*static_cast<drawing::brush **>(arg) = brush_;
		else if (access == common::property_access::write)
			brush_ = static_cast<drawing::brush *>(arg);
	}
	else if (prop == &drawer){
		if (access == common::property_access::read)
			*static_cast<drawing::object **>(arg) = drawer_;
		else if (access == common::property_access::write)
			drawer_ = static_cast<drawing::object *>(arg);
	}
	else if (prop == &draw && *static_cast<bool *>(arg)){
		stroke_.style->created = true;
		draw_();
	}
	else if (prop == &fill && *static_cast<bool *>(arg))
		fill_();
}

void ewin::drawing::shape::draw_(){}

void ewin::drawing::shape::fill_(){}

ewin::drawing::rectangle_shape::rectangle_shape(){
	position.initialize_(&position_, nullptr);
	size.initialize_(&size_, nullptr);
}

ewin::drawing::rectangle_shape::~rectangle_shape() = default;

void ewin::drawing::rectangle_shape::draw_(){
	if (drawer_ == nullptr || brush_ == nullptr)
		return;//Missing required objects

	types::render_target *native = drawer_->native;
	if (native == nullptr)
		return;//Renderer not created

	native->DrawRectangle(
		d2d1::RectF(
			(position_.x),
			(position_.y),
			(position_.x + size_.width),
			(position_.y + size_.height)
		),
		brush_->native,
		stroke_.width,
		stroke_.style->native
	);
}

void ewin::drawing::rectangle_shape::fill_(){
	types::render_target *native = drawer_->native;
	if (native == nullptr)
		return;//Renderer not created

	native->FillRectangle(
		d2d1::RectF(
		(position_.x),
			(position_.y),
			(position_.x + size_.width),
			(position_.y + size_.height)
		),
		brush_->native
	);
}
