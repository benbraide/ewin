#include "drawing_shapes.h"

ewin::drawing::object::object()
	: brush_(nullptr), began_(false), result_(NOERROR){
	auto handler = EWIN_PROP_HANDLER(object);

	cache_ = cache_info{
		types::point{},
		types::usize{},
		d2d1::IdentityMatrix(),
		types::anti_alias_mode::D2D1_ANTIALIAS_MODE_ALIASED,
		types::text_anti_alias_mode::D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
	};

	native.initialize_(nullptr, handler);
	brush.initialize_(nullptr, handler);

	dpi.initialize_(&cache_.dpi, handler);
	size.initialize_(nullptr, handler);
	pixel_size.initialize_(&cache_.pixel_size, handler);

	pixel_format.initialize_(nullptr, handler);
	maximum_bitmap_size.initialize_(nullptr, handler);
	is_supported.initialize_(nullptr, handler);

	transform.initialize_(&cache_.transform, handler);
	anti_alias_mode.initialize_(&cache_.anti_alias_mode, handler);
	text_anti_alias_mode.initialize_(&cache_.text_anti_alias_mode, handler);
	
	clear.initialize_(nullptr, handler);
	draw.initialize_(nullptr, handler);
	fill.initialize_(nullptr, handler);

	began.initialize_(nullptr, handler);
	result.initialize_(&result_, nullptr);

	created.initialize_(nullptr, handler);
	create.initialize_(nullptr, handler);
}

ewin::drawing::object::~object() = default;

void ewin::drawing::object::handle_property_(void *prop, void *arg, common::property_access access){}

void ewin::drawing::object::create_(bool create, const create_info *info){}

void ewin::drawing::object::recreate_(){}

void ewin::drawing::object::draw_(void *arg){
	auto shape = static_cast<drawing::shape *>(arg);
	if (brush_ != nullptr)//Update brush
		shape->brush = *brush_;
	shape->drawer = *this;
	shape->draw = true;
}

void ewin::drawing::object::fill_(void *arg){
	auto shape = static_cast<drawing::shape *>(arg);
	if (brush_ != nullptr)//Update brush
		shape->brush = *brush_;
	shape->drawer = *this;
	shape->fill = true;
}
