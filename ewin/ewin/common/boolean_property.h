#pragma once

#ifndef EWIN_BOOLEAN_PROPERTY_H
#define EWIN_BOOLEAN_PROPERTY_H

#include "value_property.h"

namespace ewin::common{
	template <class manager_type = void, property_access access = property_access::nil>
	class boolean_value_property : public value_property<bool, manager_type, access>{
	public:
		typedef value_property<bool, manager_type, access> base_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit boolean_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		template <typename target_type>
		boolean_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		boolean_value_property &operator =(const value_type &value){
			base_type::operator =(value);
			return *this;
		}

		bool operator !() const{
			return !(value_type)(*this);
		}

		template <typename target_type>
		bool operator ==(const target_type &rhs) const{
			return ((value_type)(*this) == (value_type)rhs);
		}

		template <typename target_type>
		bool operator !=(const target_type &rhs) const{
			return ((value_type)(*this) != (value_type)rhs);
		}

		template <typename target_type>
		friend bool operator ==(const target_type &lhs, const boolean_value_property &rhs){
			return ((value_type)lhs == (value_type)rhs);
		}

		template <typename target_type>
		friend bool operator !=(const target_type &lhs, const boolean_value_property &rhs){
			return ((value_type)lhs != (value_type)rhs);
		}
	};

	template <class manager_type = void>
	using read_only_boolean_value_property = boolean_value_property<manager_type, property_access::read>;

	template <class manager_type = void>
	using write_only_boolean_value_property = boolean_value_property<manager_type, property_access::write>;
}

#endif /* !EWIN_BOOLEAN_PROPERTY_H */
