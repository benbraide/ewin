#pragma once

#ifndef EWIN_RECT_PROPERTY_H
#define EWIN_RECT_PROPERTY_H

#include "numeric_property.h"

namespace ewin::common{
	template <class value_type>
	struct rect_value_property_backend{
		value_type left;
		value_type top;
		value_type right;
		value_type bottom;
	};

	template <class backend_value_type, class manager_type = void, property_access access = property_access::nil>
	class rect_value_property : public value_property<rect_value_property_backend<backend_value_type>, manager_type, access>{
	public:
		typedef value_property<rect_value_property_backend<backend_value_type>, manager_type, access> base_type;
		typedef numeric_value_property<backend_value_type, rect_value_property, access> numeric_value_property_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit rect_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...), left(std::forward<args_types>(args)...), top(std::forward<args_types>(args)...),
			right(std::forward<args_types>(args)...), bottom(std::forward<args_types>(args)...){}

		template <typename target_type>
		rect_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		rect_value_property &operator =(const value_type &value){
			base_type::operator =(value);
			return *this;
		}

		bool operator ==(const value_type &rhs) const{
			auto left = (value_type)(*this), right = (value_type)rhs;
			return (left.left == right.left && left.top == right.top && left.right == right.right && left.bottom == right.bottom);
		}

		bool operator !=(const value_type &rhs) const{
			return !(this == rhs);
		}

		friend bool operator ==(const value_type &lhs, const rect_value_property &rhs){
			return (rhs == lhs);
		}

		friend bool operator !=(const value_type &lhs, const rect_value_property &rhs){
			return (rhs != lhs);
		}

		numeric_value_property_type left;
		numeric_value_property_type top;
		numeric_value_property_type right;
		numeric_value_property_type bottom;

	protected:
		void initialize_(value_type *linked, callback_type callback){
			base_type::initialize_(linked, callback);
			left.initialize_(linked, callback);
			top.initialize_(linked, callback);
			right.initialize_(linked, callback);
			bottom.initialize_(linked, callback);
		}
	};

	template <class value_type, class manager_type = void>
	using read_only_rect_value_property = rect_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_rect_value_property = rect_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_RECT_PROPERTY_H */
