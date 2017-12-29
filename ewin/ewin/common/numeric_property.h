#pragma once

#ifndef EWIN_NUMERIC_PROPERTY_H
#define EWIN_NUMERIC_PROPERTY_H

#include "value_property.h"

#define EWIN_NUM_PROP_FRIEND_OP(t)																\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator +(const target_type &lhs, const t &rhs){	\
	return (rhs + lhs);																			\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator -(const target_type &lhs, const t &rhs){	\
	return ((value_type)lhs - (value_type)rhs);													\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator *(const target_type &lhs, const t &rhs){	\
	return (rhs * lhs);																			\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator /(const target_type &lhs, const t &rhs){	\
	return ((value_type)lhs / (value_type)rhs);													\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, bool) operator <(const target_type &lhs, const t &rhs){		\
	return ((value_type)lhs < (value_type)rhs);													\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, bool) operator <=(const target_type &lhs, const t &rhs){		\
	return ((value_type)lhs <= (value_type)rhs);												\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, bool) operator >=(const target_type &lhs, const t &rhs){		\
	return ((value_type)lhs >= (value_type)rhs);												\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, bool) operator >(const target_type &lhs, const t &rhs){		\
	return ((value_type)lhs > (value_type)rhs);													\
}																								\
																								\
EWIN_PROP_FRIEND_OPCOMP(t)

namespace ewin::common{
	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class numeric_value_property : public value_property<value_type, manager_type, access>{
	public:
		typedef value_property<value_type, manager_type, access> base_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit numeric_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		template <typename target_type>
		numeric_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		numeric_value_property &operator =(const value_type &value){
			base_type::operator =(value);
			return *this;
		}

		template <typename target_type>
		numeric_value_property &operator +=(const target_type &rhs){
			*this = (*this + rhs);
			return *this;
		}

		template <typename target_type>
		numeric_value_property &operator -=(const target_type &rhs){
			*this = (*this - rhs);
			return *this;
		}

		template <typename target_type>
		numeric_value_property &operator *=(const target_type &rhs){
			*this = (*this * rhs);
			return *this;
		}

		template <typename target_type>
		numeric_value_property &operator /=(const target_type &rhs){
			*this = (*this / rhs);
			return *this;
		}

		template <typename target_type>
		value_type operator +(const target_type &rhs) const{
			return ((value_type)(*this) + (value_type)rhs);
		}

		template <typename target_type>
		value_type operator -(const target_type &rhs) const{
			return ((value_type)(*this) - (value_type)rhs);
		}

		template <typename target_type>
		value_type operator *(const target_type &rhs) const{
			return ((value_type)(*this) * (value_type)rhs);
		}

		template <typename target_type>
		value_type operator /(const target_type &rhs) const{
			return ((value_type)(*this) / (value_type)rhs);
		}

		value_type operator +() const{
			return +operator value_type();
		}

		value_type operator -() const{
			return -operator value_type();
		}

		template <typename target_type>
		bool operator <(const target_type &rhs) const{
			return ((value_type)(*this) < (value_type)rhs);
		}

		template <typename target_type>
		bool operator <=(const target_type &rhs) const{
			return ((value_type)(*this) <= (value_type)rhs);
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
		bool operator >=(const target_type &rhs) const{
			return ((value_type)(*this) >= (value_type)rhs);
		}

		template <typename target_type>
		bool operator >(const target_type &rhs) const{
			return ((value_type)(*this) > (value_type)rhs);
		}

		EWIN_NUM_PROP_FRIEND_OP(numeric_value_property)
	};

	template <class value_type, class manager_type = void>
	using read_only_numeric_value_property = numeric_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_numeric_value_property = numeric_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_NUMERIC_PROPERTY_H */
