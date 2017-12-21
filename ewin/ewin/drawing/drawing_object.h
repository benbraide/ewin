#pragma once

#ifndef EWIN_DRAWING_OBJECT_H
#define EWIN_DRAWING_OBJECT_H

#include "../common/point_property.h"
#include "../common/size_property.h"
#include "../common/transformation_property.h"
#include "../common/error_handler.h"

#include "drawing_factory.h"

namespace ewin::drawing{
	class brush;
	class shape;

	class object{
	public:
		struct cache_info{
			types::point dpi;
			types::usize pixel_size;
			types::matrix_3x2 transform;
			types::anti_alias_mode anti_alias_mode;
			types::text_anti_alias_mode text_anti_alias_mode;
		};

		struct create_info{};

		object();

		virtual ~object();

		object(const object &) = delete;
		
		object &operator =(const object &) = delete;

		common::read_only_object_value_property<types::render_target, object> native;
		common::object_value_property<drawing::brush, object> brush;

		common::point_value_property<types::point, object> dpi;
		common::read_only_size_value_property<types::size, object> size;
		common::size_value_property<types::usize, object> pixel_size;

		common::read_only_value_property<types::pixel_format, object> pixel_format;
		common::read_only_value_property<unsigned __int32, object> maximum_bitmap_size;
		common::transformation_property<types::render_target_properties, bool, object> is_supported;

		common::value_property<types::matrix_3x2, object> transform;
		common::value_property<types::anti_alias_mode, object> anti_alias_mode;
		common::value_property<types::text_anti_alias_mode, object> text_anti_alias_mode;

		common::write_only_value_property<types::color, object> clear;
		common::write_only_object_value_property<drawing::shape, object> draw;
		common::write_only_object_value_property<drawing::shape, object> fill;

		common::boolean_value_property<object> began;
		common::read_only_value_property<types::result, object> result;

		common::boolean_value_property<object> created;
		common::write_only_value_property<create_info, object> create;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access);

		virtual void create_(bool create, const create_info *info);

		virtual void recreate_();

		virtual void draw_(void *arg);

		virtual void fill_(void *arg);

		drawing::brush *brush_;
		bool began_;
		types::result result_;
		cache_info cache_;
	};

	template <class native_type>
	class typed_object : public object{
	public:
		typedef native_type native_type;

		typed_object()
			: native_(nullptr){}

		virtual ~typed_object(){
			if (native_ != nullptr){
				native_->Release();
				native_ = nullptr;
			}
		}

		virtual operator types::render_target *() const{
			return native_;
		}

		template <typename unused_type = native_type>
		operator std::enable_if_t<!std::is_same_v<unused_type, types::render_target>, native_type> *() const{
			return native_;
		}

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override{
			if (prop == &brush){
				if (access == common::property_access::read)
					*static_cast<drawing::brush **>(arg) = brush_;
				else if (access == common::property_access::write)
					brush_ = static_cast<drawing::brush *>(arg);
			}
			else if (prop == &clear){
				if (native_ != nullptr)
					native_->Clear(*static_cast<types::color *>(arg));
			}
			else if (prop == &began){
				if (access == common::property_access::write){
					if (native_ != nullptr){
						if (*static_cast<bool *>(arg) && !began_){
							began_ = true;
							result_ = S_OK;
							native_->BeginDraw();
						}
						else if (!*static_cast<bool *>(arg) && began_){
							began_ = false;
							if ((result_ = native_->EndDraw()) == D2DERR_RECREATE_TARGET)
								recreate_();
						}
					}
				}
				else if (access == common::property_access::read)
					*static_cast<bool *>(arg) = began_;
			}
			else if (prop == &created){
				if (access == common::property_access::read)
					*static_cast<bool *>(arg) = (native_ != nullptr);
				else if (access == common::property_access::write)
					create_(*static_cast<bool *>(arg), nullptr);
			}
			else if (prop == &is_supported && native_ != nullptr){
				auto info = static_cast<std::pair<types::render_target_properties *, bool> *>(arg);
				info->second = EWIN_CPP_BOOL(native_->IsSupported(info->first));
			}
			else if (prop == &native)
				*static_cast<drawing::types::render_target **>(arg) = native_;
			else if (prop == &draw)
				draw_(arg);
			else if (prop == &fill)
				fill_(arg);
			else if (prop == &create)
				create_(true, static_cast<create_info *>(arg));
			else if (prop == &dpi && access == common::property_access::write && native_ != nullptr)
				native_->SetDpi(cache_.dpi.x, cache_.dpi.y);
			else if (prop == &pixel_format && native_ != nullptr)
				*static_cast<types::pixel_format *>(arg) = native_->GetPixelFormat();
			else if (prop == &maximum_bitmap_size && native_ != nullptr)
				*static_cast<unsigned __int32 *>(arg) = native_->GetMaximumBitmapSize();
			else if (prop == &transform && access == common::property_access::write && native_ != nullptr)
				native_->SetTransform(&cache_.transform);
			else if (prop == &anti_alias_mode && access == common::property_access::write && native_ != nullptr)
				native_->SetAntialiasMode(cache_.anti_alias_mode);
			else if (prop == &text_anti_alias_mode && access == common::property_access::write && native_ != nullptr)
				native_->SetTextAntialiasMode(cache_.text_anti_alias_mode);
		}

		virtual void create_(bool create, const create_info *info) override{
			if (!create && native_ != nullptr){
				native_->Release();
				native_ = nullptr;
			}
		}

		native_type *native_;
	};
}

#endif /* !EWIN_DRAWING_OBJECT_H */
