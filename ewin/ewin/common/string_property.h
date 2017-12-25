#pragma once

#ifndef EWIN_STRING_PROPERTY_H
#define EWIN_STRING_PROPERTY_H

#include <string>

#include "numeric_property.h"

#define EWIN_STR_PROP_FRIEND_OP(t)																\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, value_type) operator +(const target_type &lhs, const t &rhs){	\
	return (rhs + lhs);																			\
}																								\
																								\
EWIN_PROP_FRIEND_OPCOMP(t)

namespace ewin::common{
	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class string_value_property : public property_object{
	public:
		typedef value_type value_type;
		typedef manager_type manager_type;
		typedef property_access access_type;

		typedef typename value_type::traits_type::char_type char_type;
		typedef typename value_type::iterator iterator_type;
		typedef typename value_type::const_iterator const_iterator_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;
		typedef numeric_value_property<std::size_t, string_value_property, access_type::read> numeric_value_property_type;

		string_value_property()
			: linked_(nullptr){}

		explicit string_value_property(value_type &linked)
			: linked_(&linked){}

		explicit string_value_property(callback_type callback)
			: linked_(nullptr), callback_(callback){}

		string_value_property(value_type &linked, callback_type callback)
			: linked_(&linked), callback_(callback){}

		template <typename target_type, typename unused = value_type>
		string_value_property &operator =(const target_type &value){
			return operator =((value_type)value);
		}

		string_value_property &operator =(const string_value_property &value){
			return operator =((value_type)value);
		}

		string_value_property &operator =(const char_type *value){
			return operator =(value_type(value));
		}

		string_value_property &operator =(const value_type &value){
			if (access != access_type::nil && !EWIN_IS(access, access_type::write))
				throw error_type::property_access_violation;

			if (linked_ != nullptr){
				*linked_ = value;
				if (callback_ != nullptr)//Alert listener
					callback_(this, nullptr, access_type::write);
			}
			else if (callback_ != nullptr)//Call handler
				callback_(this, &const_cast<value_type &>(value), access_type::write);
			else//Error
				throw error_type::uninitialized_property;

			return *this;
		}

		operator value_type() const{
			if (access != access_type::nil && !EWIN_IS(access, access_type::read))
				throw error_type::property_access_violation;

			if (linked_ != nullptr){
				if (callback_ != nullptr)//Alert listener
					callback_(const_cast<string_value_property *>(this), nullptr, access_type::read);
				return *linked_;
			}

			if (callback_ != nullptr){//Call handler
				value_type value;
				callback_(const_cast<string_value_property *>(this), &value, access_type::read);
				return value;
			}

			throw error_type::uninitialized_property;//Error
		}

		string_value_property &operator +=(const value_type &rhs){
			if (access != access_type::nil && !EWIN_IS(access, access_type::read | access_type::write))
				throw error_type::property_access_violation;

			if (linked_ != nullptr){
				if (callback_ != nullptr)//Alert listener
					callback_(const_cast<string_value_property *>(this), nullptr, access_type::read);
				*linked_ += rhs;
				return *this;
			}

			if (callback_ != nullptr){//Call handler
				value_type value;
				callback_(const_cast<string_value_property *>(this), &value, access_type::read);
				return (*this = (value + rhs));
			}

			throw error_type::uninitialized_property;//Error
		}

		value_type operator +(const value_type &rhs) const{
			return ((value_type)(*this) + (value_type)rhs);
		}

		template <typename target_type>
		bool operator ==(const target_type &rhs) const{
			return ((value_type)(*this) == (value_type)rhs);
		}

		template <typename target_type>
		bool operator !=(const target_type &rhs) const{
			return ((value_type)(*this) != (value_type)rhs);
		}

		iterator_type begin(){
			if (access != access_type::nil && !EWIN_IS(access, access_type::list_begin))
				throw error_type::property_access_violation;

			if (linked_ != nullptr){
				if (callback_ != nullptr)//Alert
					callback_(this, nullptr, access_type::list_begin);
				return linked_->begin();
			}

			if (callback_ == nullptr)
				throw error_type::uninitialized_property;

			auto value = iterator_type();
			callback_(this, &value, access_type::list_begin);

			return value;
		}

		const_iterator_type begin() const{
			return const_cast<string_value_property *>(this)->begin();
		}

		iterator_type end(){
			if (access != access_type::nil && !EWIN_IS(access, access_type::list_end))
				throw error_type::property_access_violation;

			if (linked_ != nullptr){
				if (callback_ != nullptr)//Alert
					callback_(this, nullptr, access_type::list_end);
				return linked_->end();
			}

			if (callback_ == nullptr)
				throw error_type::uninitialized_property;

			auto value = iterator_type();
			callback_(this, &value, access_type::list_end);

			return value;
		}

		const_iterator_type end() const{
			return const_cast<string_value_property *>(this)->end();
		}

		numeric_value_property_type size;

		EWIN_STR_PROP_FRIEND_OP(string_value_property)

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, string_value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			auto handler = EWIN_PROP_HANDLER(string_value_property);

			linked_ = linked;
			callback_ = callback;

			size.initialize_(nullptr, handler);
		}

		void handle_property_(void *prop, void *arg, property_access access){
			if (prop == &size){//Fetch size
				if (access != access_type::nil && !EWIN_IS(access, access_type::list_size))
					throw error_type::property_access_violation;

				if (linked_ != nullptr){
					*reinterpret_cast<std::size_t *>(arg) = linked_->size();
					if (callback_ != nullptr)//Alert
						callback_(this, nullptr, property_access::list_size);
				}
				else if (callback_ != nullptr)
					callback_(this, arg, property_access::list_size);
				else//Error
					throw error_type::uninitialized_property;
			}
		}

		value_type *linked_;
		callback_type callback_;
	};

	template <class value_type, class manager_type = void>
	using read_only_string_value_property = string_value_property<
		value_type,
		manager_type,
		property_access::read | property_access::list_begin | property_access::list_end | property_access::list_size
	>;

	template <class value_type, class manager_type = void>
	using write_only_string_value_property = string_value_property<value_type, manager_type, property_access::write>;

	template <class value_type, class manager_type = void>
	using iterator_only_string_value_property = string_value_property<
		value_type,
		manager_type,
		property_access::list_begin | property_access::list_end
	>;
}

#endif /* !EWIN_STRING_PROPERTY_H */
