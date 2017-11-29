#pragma once

#ifndef EWIN_INTEGRAL_PROPERTY_H
#define EWIN_INTEGRAL_PROPERTY_H

#include "numeric_property.h"

namespace ewin::common{
	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class integral_value_property : public numeric_value_property<value_type, manager_type, access>{
	public:
		typedef numeric_value_property<value_type, manager_type, access> base_type;

		using base_type::operator value_type;

		using base_type::operator +;
		using base_type::operator -;
		using base_type::operator *;
		using base_type::operator /;

		using base_type::operator <;
		using base_type::operator <=;
		using base_type::operator ==;
		using base_type::operator !=;
		using base_type::operator >=;
		using base_type::operator >;

		template <typename... args_types>
		explicit integral_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		template <typename target_type>
		integral_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		integral_value_property &operator =(const value_type &value){
			base_type::operator =(value);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator +=(const target_type &rhs){
			*this = (*this + rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator -=(const target_type &rhs){
			*this = (*this - rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator *=(const target_type &rhs){
			*this = (*this * rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator /=(const target_type &rhs){
			*this = (*this / rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator %=(const target_type &rhs){
			*this = (*this % rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator <<=(const target_type &rhs){
			*this = (*this << rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator >>=(const target_type &rhs){
			*this = (*this >> rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator &=(const target_type &rhs){
			*this = (*this & rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator ^=(const target_type &rhs){
			*this = (*this ^ rhs);
			return *this;
		}

		template <typename target_type>
		integral_value_property &operator |=(const target_type &rhs){
			*this = (*this | rhs);
			return *this;
		}

		template <typename target_type>
		friend value_type operator +(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs + (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator -(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs - (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator *(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs * (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator /(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs / (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator %(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs % (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator <<(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs << (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator >>(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs >> (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator &(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs & (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator ^(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs ^ (value_type)rhs);
		}

		template <typename target_type>
		friend value_type operator |(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs | (value_type)rhs);
		}

		template <typename target_type>
		friend bool operator <(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs < (value_type)rhs);
		}

		template <typename target_type>
		friend bool operator <=(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs <= (value_type)rhs);
		}

		template <typename target_type>
		friend bool operator ==(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs == (value_type)rhs);
		}

		template <typename target_type>
		friend bool operator !=(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs != (value_type)rhs);
		}

		template <typename target_type>
		friend bool operator >=(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs >= (value_type)rhs);
		}

		template <typename target_type>
		friend bool operator >(const target_type &lhs, const integral_value_property &rhs){
			return ((value_type)lhs > (value_type)rhs);
		}
	};

	template <class value_type, class manager_type = void>
	using read_only_integral_value_property = integral_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_integral_value_property = integral_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_INTEGRAL_PROPERTY_H */
