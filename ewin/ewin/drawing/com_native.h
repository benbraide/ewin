#pragma once

#ifndef EWIN_COM_NATIVE_H
#define EWIN_COM_NATIVE_H

#include "../common/boolean_property.h"
#include "../common/object_property.h"

namespace ewin::drawing{
	template <class native_type>
	class com_native{
	public:
		typedef native_type native_type;

		struct create_info{};

		com_native()
			: native_(nullptr){
			native.initialize_(native_, nullptr);
			created.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				if (access == common::property_access::write)
					create_(*static_cast<bool *>(arg), nullptr);
				else if (access == common::property_access::read)
					*static_cast<bool *>(arg) = (native_ != nullptr);
			});
		}

		virtual ~com_native(){
			if (native_ != nullptr){
				native_->Release();
				native_ = nullptr;
			}
		}

		com_native(const com_native &) = delete;

		com_native &operator =(const com_native &) = delete;

		common::read_only_object_value_property<native_type, com_native> native;
		common::boolean_value_property<com_native> created;

	protected:
		virtual void create_(bool create, const create_info *info){
			if (!create && native_ != nullptr){
				native_->Release();
				native_ = nullptr;
			}
		}

		native_type *native_;
	};
}

#endif /* !EWIN_COM_NATIVE_H */
