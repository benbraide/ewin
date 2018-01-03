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
			native.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				*static_cast<native_type **>(arg) = native_;
			});

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

	template <class native_type>
	class shared_com_native : public com_native<native_type>{
	public:
		explicit shared_com_native(native_type *value){
			native_ = value;
		}

		shared_com_native(const shared_com_native &other){
			if ((native_ = other.native_) != nullptr)
				native_->AddRef();
		}

		shared_com_native &operator =(const shared_com_native &other){
			if (native_ != nullptr)//Release previous
				native_->Release();

			if ((native_ = other.native_) != nullptr)
				native_->AddRef();

			return *this;
		}
	};
}

#endif /* !EWIN_COM_NATIVE_H */
