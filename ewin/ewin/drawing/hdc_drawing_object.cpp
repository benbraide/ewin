#include "hdc_drawing_object.h"

ewin::drawing::hdc_object::hdc_object()
	: factory_(nullptr), target_(nullptr), target_locked_(false){
	offset_ = common::types::point{};
	bind_properties_();
}

ewin::drawing::hdc_object::hdc_object(const create_info &info)
	: factory_(nullptr), target_(nullptr), target_locked_(false){
	bind_properties_();
}

ewin::drawing::hdc_object::~hdc_object() = default;

void ewin::drawing::hdc_object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(hdc_object);

	factory.initialize_(nullptr, handler);
	target.initialize_(nullptr, handler);
	offset.initialize_(&offset_, handler);
	lock_target.initialize_(nullptr, handler);
}

void ewin::drawing::hdc_object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &factory){
		if (access == common::property_access::read)
			*static_cast<drawing::factory **>(arg) = factory_;
		else if (access == common::property_access::write && native_ == nullptr)
			factory_ = static_cast<drawing::factory *>(arg);
	}
	else if (prop == &target){
		if (access == common::property_access::write && !target_locked_ && *static_cast<common::types::hdc *>(arg) != target_){
			target_ = *static_cast<common::types::hdc *>(arg);
			if (native_ != nullptr){//Update
				common::types::rect rect{
					(offset_.x),
					(offset_.y),
					(offset_.x + static_cast<int>(cache_.pixel_size.width)),
					(offset_.y + static_cast<int>(cache_.pixel_size.height))
				};

				native_->BindDC(target_, &rect);
			}
		}
		else if (access == common::property_access::read)
			*static_cast<common::types::hdc *>(arg) = target_;
	}
	else if ((prop == &pixel_size || prop == &offset) && access == common::property_access::write && native_ != nullptr && target_ != nullptr){//Update
		common::types::rect rect{
			(offset_.x),
			(offset_.y),
			(offset_.x + static_cast<int>(cache_.pixel_size.width)),
			(offset_.y + static_cast<int>(cache_.pixel_size.height))
		};

		native_->BindDC(target_, &rect);
	}
	else if (prop == &lock_target && *static_cast<bool *>(arg))
		target_locked_ = true;
	else//Forward
		base_type::handle_property_(prop, arg, access);
}

void ewin::drawing::hdc_object::create_(bool create, const base_type::create_info *info){
	if (create && native_ == nullptr){
		if (info == nullptr && factory_ != nullptr && target_ != nullptr){
			auto properties = d2d1::RenderTargetProperties(
				types::render_target_type::D2D1_RENDER_TARGET_TYPE_DEFAULT,
				d2d1::PixelFormat(
					types::dxgi_format::DXGI_FORMAT_B8G8R8A8_UNORM,
					types::alpha_mode::D2D1_ALPHA_MODE_PREMULTIPLIED
				),
				cache_.dpi.x,
				cache_.dpi.y,
				types::render_target_usage::D2D1_RENDER_TARGET_USAGE_NONE,
				types::feature_level::D2D1_FEATURE_LEVEL_DEFAULT
			);

			auto result = factory_->native->CreateDCRenderTarget(
				&properties,
				&native_
			);

			if (SUCCEEDED(result) && native_ != nullptr && target_ != nullptr){//Bind
				native_->GetDpi(&cache_.dpi.x, &cache_.dpi.y);
				common::types::rect rect{
					(offset_.x),
					(offset_.y),
					(offset_.x + static_cast<int>(cache_.pixel_size.width)),
					(offset_.y + static_cast<int>(cache_.pixel_size.height))
				};

				native_->BindDC(target_, &rect);
			}
		}
		else if (info != nullptr)//Use info
			low_level_create_(*reinterpret_cast<const create_info *>(info));
	}
	else//Forward
		base_type::create_(create, info);
}

void ewin::drawing::hdc_object::recreate_(){
	create_(false, nullptr);//Destroy
	create_(true, nullptr);//Create
}

void ewin::drawing::hdc_object::low_level_create_(const create_info &info){}
