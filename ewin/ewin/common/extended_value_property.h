#pragma once

#ifndef EWIN_EXTENDED_VALUE_PROPERTY_H
#define EWIN_EXTENDED_VALUE_PROPERTY_H

#include "value_property.h"

namespace ewin::common{
	template <class value_type, class read_type, class manager_type = void, property_access access = property_access::nil>
	class extended_value_property{
	public:
		typedef value_type value_type;
		typedef read_type read_type;
		typedef manager_type manager_type;
		typedef property_access access_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;

		extended_value_property() = default;

		explicit extended_value_property(callback_type callback)
			: callback_(callback){}

		template <typename target_type>
		extended_value_property &operator =(const target_type &value){
			return operator =((read_type)value);
		}

		extended_value_property &operator =(const value_type &value){
			if (access != access_type::nil && !EWIN_IS(access, access_type::write))
				throw error_type::property_access_violation;

			if (callback_ != nullptr)//Call handler
				callback_(this, &const_cast<value_type &>(value), access_type::write);
			else//Error
				throw error_type::uninitialized_property;

			return *this;
		}

		extended_value_property &operator =(const read_type &value){
			if (access != access_type::nil && !EWIN_IS(access, access_type::write))
				throw error_type::property_access_violation;

			if (callback_ != nullptr)//Call handler
				callback_(this, &const_cast<read_type &>(value), access_type::write_alt);
			else//Error
				throw error_type::uninitialized_property;

			return *this;
		}

		operator read_type() const{
			if (access != access_type::nil && !EWIN_IS(access, access_type::read))
				throw error_type::property_access_violation;

			if (callback_ != nullptr){//Call handler
				auto value = read_type();
				callback_(const_cast<extended_value_property *>(this), &value, access_type::read);
				return value;
			}

			throw error_type::uninitialized_property;//Error
		}

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, extended_value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			callback_ = callback;
		}

		callback_type callback_;
	};
}

#endif /* !EWIN_EXTENDED_VALUE_PROPERTY_H */
