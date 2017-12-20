#include "../window/window_object.h"

ewin::drawing::hwnd_object::hwnd_object()
	: factory_(nullptr), target_(nullptr), size_event_id_(0), destroy_event_id_(0), target_locked_(false){
	bind_properties_();
}

ewin::drawing::hwnd_object::hwnd_object(const create_info &info)
	: factory_(nullptr), target_(nullptr), size_event_id_(0), destroy_event_id_(0), target_locked_(false){
	bind_properties_();
}

ewin::drawing::hwnd_object::~hwnd_object() = default;

void ewin::drawing::hwnd_object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(hwnd_object);

	factory.initialize_(nullptr, handler);
	target.initialize_(nullptr, handler);
	lock_target.initialize_(nullptr, handler);
}

void ewin::drawing::hwnd_object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &factory){
		if (access == common::property_access::read)
			*static_cast<drawing::factory **>(arg) = factory_;
		else if (access == common::property_access::write && native_ == nullptr)
			factory_ = static_cast<drawing::factory *>(arg);
	}
	else if (prop == &target){
		if (access == common::property_access::write && !target_locked_ && static_cast<window::object *>(arg) != target_ && native_ == nullptr){
			if (target_ != nullptr){
				if (size_event_id_ != 0u){//Unbind previous
					target_->events->position_change -= size_event_id_;
					size_event_id_ = 0;
				}

				if (destroy_event_id_ != 0u){//Unbind previous
					target_->events->destroy -= destroy_event_id_;
					destroy_event_id_ = 0;
				}
			}
			
			target_ = static_cast<window::object *>(arg);
		}
		else if (access == common::property_access::read)
			*static_cast<window::object **>(arg) = target_;
	}
	else if (prop == &pixel_size && access == common::property_access::write && native_ != nullptr)//Update
		native_->Resize(cache_.pixel_size);
	else if (prop == &lock_target && *static_cast<bool *>(arg))
		target_locked_ = true;
	else//Forward
		base_type::handle_property_(prop, arg, access);
}

void ewin::drawing::hwnd_object::create_(bool create, const base_type::create_info *info){
	if (create && native_ == nullptr){
		if (info == nullptr && target_ != nullptr && target_->created){
			cache_.pixel_size = types::usize{
				target_->client_size.width,
				target_->client_size.height
			};

			if (factory_ == nullptr)
				factory_ = target_->app->drawing_factory;

			target_->app->task += [this]{
				auto result = factory_->native->CreateHwndRenderTarget(
					d2d1::RenderTargetProperties(
						types::render_target_type::D2D1_RENDER_TARGET_TYPE_DEFAULT,
						d2d1::PixelFormat(
							types::dxgi_format::DXGI_FORMAT_UNKNOWN,
							types::alpha_mode::D2D1_ALPHA_MODE_UNKNOWN
						),
						cache_.dpi.x,
						cache_.dpi.y,
						types::render_target_usage::D2D1_RENDER_TARGET_USAGE_NONE,
						types::feature_level::D2D1_FEATURE_LEVEL_DEFAULT
					),
					d2d1::HwndRenderTargetProperties(
						target_->handle,
						cache_.pixel_size,
						types::present_options::D2D1_PRESENT_OPTIONS_NONE
					),
					&native_
				);

				if (SUCCEEDED(result) && native_ != nullptr){//Created render
					if (size_event_id_ == 0u){
						size_event_id_ = target_->events->position_change += [this](events::position_change &e){//Listen for size events
							if (target_ == nullptr || native_ == nullptr)
								return;//Basic checks

							types::usize size{
								target_->client_size.width,
								target_->client_size.height
							};

							if (!e.is_changing && size.width != cache_.pixel_size.width && size.height != cache_.pixel_size.height)
								native_->Resize(size);//Apply new size
						};
					}

					if (destroy_event_id_ == 0u){
						destroy_event_id_ = target_->events->destroy += [this]{//Listen for destruction events
							create_(false, nullptr);//Destroy
						};
					}
				}
			};
		}
		else if (info != nullptr)//Use info
			low_level_create_(*reinterpret_cast<const create_info *>(info));
	}
	else if (!create && native_ != nullptr){
		if (factory_ != nullptr){
			factory_->app->task += [this]{
				native_->Release();
				native_ = nullptr;
			};
		}
		else{//Release in current context
			native_->Release();
			native_ = nullptr;
		}
	}
}

void ewin::drawing::hwnd_object::recreate_(){
	create_(false, nullptr);//Destroy
	create_(true, nullptr);//Create
}

void ewin::drawing::hwnd_object::low_level_create_(const create_info &info){}
