#include "../application/application_object.h"

ewin::drawing::factory::factory()
	: app_(nullptr), native_(nullptr){
	auto handler = EWIN_PROP_HANDLER(factory);
	app.initialize_(nullptr, handler);
	native.initialize_(nullptr, handler);
	created.initialize_(nullptr, handler);
}

ewin::drawing::factory::~factory(){
	if (native_ != nullptr){
		native_->Release();
		native_ = nullptr;
	}
}

void ewin::drawing::factory::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &created){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = (native_ != nullptr);
		else if (access == common::property_access::write)
			create_(*static_cast<bool *>(arg), nullptr);
	}
	else if (prop == &app)
		*static_cast<application::object **>(arg) = app_;
	else if (prop == &native)
		*static_cast<drawing::types::factory **>(arg) = native_;
}

void ewin::drawing::factory::create_(bool create, const create_info *info){
	if (create && native_ == nullptr){
		if (app_ != nullptr){//Execute in application's context
			app_->task += [&]{
				::D2D1CreateFactory(types::factory_type::D2D1_FACTORY_TYPE_SINGLE_THREADED, &native_);
			};
		}
		else//Create in current context
			::D2D1CreateFactory(types::factory_type::D2D1_FACTORY_TYPE_SINGLE_THREADED, &native_);
	}
	else if (!create && native_ != nullptr){
		if (app_ != nullptr){//Execute in application's context
			app_->task += [&]{
				native_->Release();
			};
		}
		else//Release in current context
			native_->Release();
		
		native_ = nullptr;
	}
}
