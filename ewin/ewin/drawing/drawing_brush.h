#pragma once

#ifndef EWIN_DRAWING_BRUSH_H
#define EWIN_DRAWING_BRUSH_H

#include "drawing_object.h"

namespace ewin::drawing{
	class brush{
	public:
		struct create_info{};

		brush()
			: opacity_(1.0f), transform_(d2d1::IdentityMatrix()){
			auto handler = EWIN_PROP_HANDLER(brush);

			native.initialize_(nullptr, handler);

			opacity.initialize_(&opacity_, handler);
			transform.initialize_(&transform_, handler);

			created.initialize_(nullptr, handler);
			create.initialize_(nullptr, handler);
		}

		virtual ~brush() = default;

		brush(const brush &) = delete;
		
		brush &operator =(const brush &) = delete;

		common::read_only_object_value_property<types::brush, brush> native;

		common::value_property<float, brush> opacity;
		common::value_property<types::matrix_3x2, brush> transform;

		common::boolean_value_property<brush> created;
		common::write_only_value_property<create_info, brush> create;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access){}

		float opacity_;
		types::matrix_3x2 transform_;
	};

	template <class native_type>
	class typed_brush : public brush{
	public:
		typedef native_type native_type;

		typed_brush()
			: native_(nullptr){}

		virtual ~typed_brush(){
			create_(false, nullptr);
		}

		virtual operator types::brush *() const{
			return native_;
		}

		template <typename unused_type = native_type>
		operator std::enable_if_t<!std::is_same_v<unused_type, types::brush>, native_type> *() const{
			return native_;
		}

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override{
			if (prop == &created){
				if (access == common::property_access::read)
					*static_cast<bool *>(arg) = (native_ != nullptr);
				else if (access == common::property_access::write)
					create_(*static_cast<bool *>(arg), nullptr);
			}
			else if (prop == &native)
				*static_cast<drawing::types::brush **>(arg) = native_;
			else if (prop == &opacity && access == common::property_access::write && native_ != nullptr)//Update
				native_->SetOpacity(opacity_);
			else if (prop == &transform && access == common::property_access::write && native_ != nullptr)//Update
				native_->SetTransform(transform_);
			else if (prop == &create)
				create_(true, static_cast<create_info *>(arg));
		}

		virtual void create_(bool create, const create_info *info){
			if (!create && native_ != nullptr){
				native_->Release();
				native_ = nullptr;
			}
		}

		native_type *native_;
	};
}

#endif /* !EWIN_DRAWING_BRUSH_H */
