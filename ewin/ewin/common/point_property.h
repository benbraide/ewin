#pragma once

#ifndef EWIN_POINT_PROPERTY_H
#define EWIN_POINT_PROPERTY_H

#include "numeric_property.h"

namespace ewin::common{
	template <class value_type>
	struct point_value_property_backend{
		value_type x;
		value_type y;
	};

	template <class backend_value_type, class manager_type = void, property_access access = property_access::nil>
	class point_value_property : public value_property<point_value_property_backend<backend_value_type>, manager_type, access>{
	public:
		typedef value_property<point_value_property_backend<backend_value_type>, manager_type, access> base_type;
		typedef numeric_value_property<backend_value_type, point_value_property, access> numeric_value_property_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit point_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...), x(std::forward<args_types>(args)...), y(std::forward<args_types>(args)...){}

		template <typename target_type>
		point_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		point_value_property &operator =(const value_type &value){
			base_type::operator =(value);
			return *this;
		}

		bool operator ==(const value_type &rhs) const{
			auto left = (value_type)(*this), right = (value_type)rhs;
			return (left.x == right.x && left.y == right.y);
		}

		bool operator !=(const value_type &rhs) const{
			return !(this == rhs);
		}

		friend bool operator ==(const value_type &lhs, const point_value_property &rhs){
			return (rhs == lhs);
		}

		friend bool operator !=(const value_type &lhs, const point_value_property &rhs){
			return (rhs != lhs);
		}

		numeric_value_property_type x;
		numeric_value_property_type y;

	protected:
		void initialize_(value_type *linked, callback_type callback){
			base_type::initialize_(linked, callback);
			x.initialize_(nullptr, callback);
			y.initialize_(nullptr, callback);
		}
	};

	template <class value_type, class manager_type = void>
	using read_only_point_value_property = point_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_point_value_property = point_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_POINT_PROPERTY_H */
