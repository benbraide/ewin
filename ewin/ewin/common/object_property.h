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

		typedef property_error error_type;
		typedef property_access access_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;

		object_value_property()
			: linked_(nullptr){}

		explicit object_value_property(value_type &linked)
			: linked_(&linked){}

		value_type *operator ->() const{
			if (linked_ == nullptr)
				throw error_type::uninitialized;
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
	};
}

#endif /* !EWIN_OBJECT_PROPERTY_H */
