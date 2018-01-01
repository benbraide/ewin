#include "../application/application_object.h"

ewin::writing::factory::factory()
	: app_(nullptr), native_(nullptr){
	auto handler = EWIN_PROP_HANDLER(factory);
	app.initialize_(nullptr, handler);
	native.initialize_(nullptr, handler);
	created.initialize_(nullptr, handler);
}

ewin::writing::factory::~factory(){
	create_(false, nullptr);
}

void ewin::writing::factory::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &created){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = (native_ != nullptr);
		else if (access == common::property_access::write)
			create_(*static_cast<bool *>(arg), nullptr);
	}
	else if (prop == &app)
		*static_cast<application::object **>(arg) = app_;
	else if (prop == &native)
		*static_cast<writing::types::factory **>(arg) = native_;
}

void ewin::writing::factory::create_(bool create, const create_info *info){
	if (create && native_ == nullptr){
		if (app_ != nullptr){//Execute in application's context
			app_->task += [&]{
				::DWriteCreateFactory(types::factory_type::DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(types::factory), reinterpret_cast<types::unknown **>(&native_));
			};
		}
		else//Create in current context
			::DWriteCreateFactory(types::factory_type::DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(types::factory), reinterpret_cast<types::unknown **>(&native_));
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
