#include "solid_color_drawing_brush.h"

ewin::drawing::solid_color_brush::solid_color_brush()
	: drawer_(nullptr), color_(d2d1::ColorF(d2d1::ColorF::Black)){
	bind_properties_();
}

ewin::drawing::solid_color_brush::solid_color_brush(object &drawer, const types::color &color)
	: drawer_(nullptr), color_(d2d1::ColorF(d2d1::ColorF::Black)){
	bind_properties_();
	low_level_create_(create_info{ drawer, color });
}

ewin::drawing::solid_color_brush::~solid_color_brush() = default;

void ewin::drawing::solid_color_brush::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(solid_color_brush);

	drawer.initialize_(nullptr, handler);
	color.initialize_(&color_, handler);
}

void ewin::drawing::solid_color_brush::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &drawer){
		if (access == common::property_access::read)
			*static_cast<object **>(arg) = drawer_;
		else if (access == common::property_access::write && native_ == nullptr)
			drawer_ = static_cast<object *>(arg);
	}
	else if (prop == &color && access == common::property_access::write && native_ != nullptr){//Update
		native_->SetColor(color_);
		opacity_ = native_->GetOpacity();
	}
	else//Forward
		base_type::handle_property_(prop, arg, access);
}

void ewin::drawing::solid_color_brush::create_(bool create, const base_type::create_info *info){
	if (create && native_ == nullptr){
		if (info == nullptr && drawer_ != nullptr){
			low_level_create_(create_info{
				*drawer_,
				color_,
				opacity_,
				transform_
			});
		}
		else if (info != nullptr)//Use info
			low_level_create_(*reinterpret_cast<const create_info *>(info));
	}
	else//Forward
		base_type::create_(create, info);
}

void ewin::drawing::solid_color_brush::low_level_create_(const create_info &info){
	info.drawer.get().native->CreateSolidColorBrush(
		info.color,
		d2d1::BrushProperties(info.opacity, info.transform),
		&native_
	);
}
