#pragma once

#ifndef EWIN_OBJECT_PROPERTY_H
#define EWIN_OBJECT_PROPERTY_H

#include "value_property.h"

namespace ewin::common{
	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class object_value_property{
	public:
		typedef value_type value_type;
		typedef manager_type manager_type;
		typedef property_access access_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;

		object_value_property()
			: linked_(nullptr){}

		explicit object_value_property(value_type &linked)
			: linked_(&linked){}

		explicit object_value_property(callback_type callback)
			: linked_(nullptr), callback_(callback){}

		object_value_property(value_type &linked, callback_type callback)
			: linked_(&linked), callback_(callback){}

		value_type *operator ->() const{
			if (linked_ == nullptr){
				if (callback_ == nullptr)
					throw error_type::uninitialized_property;

				value_type *value = nullptr;
				callback_(const_cast<object_value_property *>(this), &value, access_type::read);
				if (value == nullptr)
					throw error_type::uninitialized_property;

				return value;
			}

			return linked_;
		}

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, object_value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			linked_ = linked;
		}

		value_type *linked_;
		callback_type callback_;
	};
}

#endif /* !EWIN_OBJECT_PROPERTY_H */
