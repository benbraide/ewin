#pragma once

#ifndef EWIN_DRAWING_OBJECT_H
#define EWIN_DRAWING_OBJECT_H

#include "../common/point_property.h"
#include "../common/size_property.h"
#include "../common/transformation_property.h"
#include "../common/error_handler.h"

#include "drawing_factory.h"

namespace ewin::drawing{
	class object{
	public:
		object() = default;

		virtual ~object() = default;

		object(const object &) = delete;
		
		object &operator =(const object &) = delete;

		common::read_only_object_value_property<types::render_target, object> native;

	protected:
		void bind_properties_(types::render_target *native_value){
			native.initialize_(native_value, nullptr);;
		}
	};

	template <class native_type>
	class typed_object : public object{
	public:
		typedef native_type native_type;

		struct create_info{};

		struct cache_info{
			types::point dpi;
			types::usize pixel_size;
			types::matrix_3x2 transform;
			types::anti_alias_mode anti_alias_mode;
			types::text_anti_alias_mode text_anti_alias_mode;
		};

		typed_object()
			: native_(nullptr), began_(false), result_(NOERROR){
			object::bind_properties_(native_);
			auto handler = EWIN_PROP_HANDLER(typed_object);

			cache_ = cache_info{
				types::point{},
				types::usize{},
				d2d1::IdentityMatrix(),
				types::anti_alias_mode::D2D1_ANTIALIAS_MODE_ALIASED,
				types::text_anti_alias_mode::D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
			};

			dpi.initialize_(&cache_.dpi, handler);
			size.initialize_(nullptr, handler);
			pixel_size.initialize_(&cache_.pixel_size, handler);

			pixel_format.initialize_(nullptr, handler);
			maximum_bitmap_size.initialize_(nullptr, handler);
			is_supported.initialize_(nullptr, handler);

			transform.initialize_(&cache_.transform, handler);
			anti_alias_mode.initialize_(&cache_.anti_alias_mode, handler);
			text_anti_alias_mode.initialize_(&cache_.text_anti_alias_mode, handler);

			began.initialize_(&began_, handler);
			result.initialize_(&result_, handler);

			created.initialize_(nullptr, handler);
			create.initialize_(nullptr, handler);
		}

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

		common::point_value_property<types::point, typed_object> dpi;
		common::read_only_size_value_property<types::size, typed_object> size;
		common::size_value_property<types::usize, typed_object> pixel_size;

		common::read_only_value_property<types::pixel_format, typed_object> pixel_format;
		common::read_only_value_property<unsigned __int32, typed_object> maximum_bitmap_size;
		common::transformation_property<types::render_target_properties, bool, typed_object> is_supported;

		common::value_property<types::matrix_3x2, typed_object> transform;
		common::value_property<types::anti_alias_mode, typed_object> anti_alias_mode;
		common::value_property<types::text_anti_alias_mode, typed_object> text_anti_alias_mode;

		common::boolean_value_property<typed_object> began;
		common::read_only_value_property<types::result, typed_object> result;

		common::boolean_value_property<typed_object> created;
		common::write_only_value_property<create_info, typed_object> create;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access){
			if (prop == &created){
				if (access == common::property_access::read)
					*static_cast<bool *>(arg) = (native_ != nullptr);
				else if (access == common::property_access::write)
					create_(*static_cast<bool *>(arg), nullptr);
			}
			else if (prop == &began && access == common::property_access::write && native_ != nullptr){
				if (*static_cast<bool *>(arg) && !began_){
					result_ = NOERROR;
					native_->BeginDraw();
				}
				else if (!*static_cast<bool *>(arg) && began_ && (result_ = native_->EndDraw()) == D2DERR_RECREATE_TARGET)
					recreate_();
			}
			else if (prop == &is_supported && native_ != nullptr){
				auto info = static_cast<std::pair<types::render_target_properties *, bool> *>(arg);
				info->second = EWIN_CPP_BOOL(native_->IsSupported(info->first));
			}
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
			else if (prop == &create)
				create_(true, static_cast<create_info *>(arg));
		}

		virtual void create_(bool create, const create_info *info){
			if (!create && native_ != nullptr){
				native_->Release();
				native_ = nullptr;
			}
		}

		virtual void recreate_(){}

		native_type *native_;
		cache_info cache_;
		bool began_;
		types::result result_;
	};
}

#endif /* !EWIN_DRAWING_OBJECT_H */
