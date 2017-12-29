#pragma once

#ifndef EWIN_INTEGRAL_PROPERTY_H
#define EWIN_INTEGRAL_PROPERTY_H

#include "numeric_property.h"

#define EWIN_INT_PROP_FRIEND_OP(t)																\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator %(const target_type &lhs, const t &rhs){	\
	return ((value_type)lhs % (value_type)rhs);													\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator <<(const target_type &lhs, const t &rhs){\
	return ((value_type)lhs << (value_type)rhs);												\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator >>(const target_type &lhs, const t &rhs){\
	return (rhs >> lhs);																		\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator &(const target_type &lhs, const t &rhs){	\
	return ((value_type)lhs & (value_type)rhs);													\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator ^(const target_type &lhs, const t &rhs){	\
	return ((value_type)lhs ^ (value_type)rhs);													\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator |(const target_type &lhs, const t &rhs){	\
	return ((value_type)lhs | (value_type)rhs);													\
}																								\
																								\
EWIN_NUM_PROP_FRIEND_OP(t)

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
		value_type operator %(const target_type &rhs) const{
			return ((value_type)(*this) % (value_type)rhs);
		}

		template <typename target_type>
		value_type operator <<(const target_type &rhs) const{
			return ((value_type)(*this) << (value_type)rhs);
		}

		template <typename target_type>
		value_type operator >>(const target_type &rhs) const{
			return ((value_type)(*this) >> (value_type)rhs);
		}

		template <typename target_type>
		value_type operator &(const target_type &rhs) const{
			return ((value_type)(*this) & (value_type)rhs);
		}

		template <typename target_type>
		value_type operator ^(const target_type &rhs) const{
			return ((value_type)(*this) ^ (value_type)rhs);
		}

		template <typename target_type>
		value_type operator |(const target_type &rhs) const{
			return ((value_type)(*this) | (value_type)rhs);
		}

		value_type operator ~() const{
			return ~operator value_type();
		}

		integral_value_property &operator ++(){
			return (*this += static_cast<value_type>(1));
		}

		value_type operator ++(int){
			auto value = operator value_type();
			*this += static_cast<value_type>(1);
			return value;
		}

		integral_value_property &operator --(){
			return (*this -= static_cast<value_type>(1));
		}

		value_type operator --(int){
			auto value = operator value_type();
			*this -= static_cast<value_type>(1);
			return value;
		}

		EWIN_INT_PROP_FRIEND_OP(integral_value_property)
	};

	template <class value_type, class manager_type = void>
	using read_only_integral_value_property = integral_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_integral_value_property = integral_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_INTEGRAL_PROPERTY_H */
