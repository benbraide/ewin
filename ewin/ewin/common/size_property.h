#pragma once

#ifndef EWIN_SIZE_PROPERTY_H
#define EWIN_SIZE_PROPERTY_H

#include "numeric_property.h"

namespace ewin::common{
	template <class value_type>
	struct size_value_property_backend{
		value_type width;
		value_type height;
	};

	template <class backend_value_type, class manager_type = void, property_access access = property_access::nil>
	class size_value_property : public value_property<size_value_property_backend<backend_value_type>, manager_type, access>{
	public:
		typedef value_property<size_value_property_backend<backend_value_type>, manager_type, access> base_type;
		typedef numeric_value_property<backend_value_type, size_value_property, access> numeric_value_property_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit size_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...), width(std::forward<args_types>(args)...), height(std::forward<args_types>(args)...){}

		template <typename target_type>
		size_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		size_value_property &operator =(const value_type &value){
			base_type::operator =(value);
			return *this;
		}

		bool operator ==(const value_type &rhs) const{
			auto left = (value_type)(*this), right = (value_type)rhs;
			return (left.width == right.width && left.height == right.height);
		}

		bool operator !=(const value_type &rhs) const{
			return !(this == rhs);
		}

		friend bool operator ==(const value_type &lhs, const size_value_property &rhs){
			return (rhs == lhs);
		}

		friend bool operator !=(const value_type &lhs, const size_value_property &rhs){
			return (rhs != lhs);
		}

		numeric_value_property_type width;
		numeric_value_property_type height;

	protected:
		void initialize_(value_type *linked, callback_type callback){
			base_type::initialize_(linked, callback);
			width.initialize_(nullptr, callback);
			height.initialize_(nullptr, callback);
		}
	};

	template <class value_type, class manager_type = void>
	using read_only_size_value_property = size_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_size_value_property = size_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_SIZE_PROPERTY_H */
