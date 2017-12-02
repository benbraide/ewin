#pragma once

#ifndef EWIN_STRING_PROPERTY_H
#define EWIN_STRING_PROPERTY_H

#include <string>

#include "numeric_property.h"

namespace ewin::common{
	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class string_value_property{
	public:
		typedef value_type value_type;
		typedef manager_type manager_type;

		typedef property_error error_type;
		typedef property_access access_type;

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

		template <typename target_type>
		string_value_property &operator =(const target_type &value){
			return operator =((value_type)value);
		}

		string_value_property &operator =(const value_type &value){
			if (access != access_type::nil && !EWIN_IS(access, access_type::write))
				throw error_type::access_violation;

			if (linked_ != nullptr){
				*linked_ = value;
				if (callback_ != nullptr)//Alert listener
					callback_(this, nullptr, access_type::write);
			}
			else if (callback_ != nullptr)//Call handler
				callback_(this, &const_cast<value_type &>(value), access_type::write);
			else//Error
				throw error_type::uninitialized;

			return *this;
		}

		operator value_type() const{
			if (access != access_type::nil && !EWIN_IS(access, access_type::read))
				throw error_type::access_violation;

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

			throw error_type::uninitialized;//Error
		}

		string_value_property &operator +=(const value_type &rhs){
			if (access != access_type::nil && !EWIN_IS(access, access_type::read | access_type::write))
				throw error_type::access_violation;

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

			throw error_type::uninitialized;//Error
		}

		value_type operator +(const value_type &rhs) const{
			return ((value_type)(*this) + (value_type)rhs);
		}

		typename value_type::iterator begin(){
			if (linked_ == nullptr)
				throw error_type::forbidden;
			return linked_->begin();
		}

		typename value_type::const_iterator begin() const{
			if (linked_ == nullptr)
				throw error_type::forbidden;
			return linked_->begin();
		}

		typename value_type::iterator end(){
			if (linked_ == nullptr)
				throw error_type::forbidden;
			return linked_->end();
		}

		typename value_type::const_iterator end() const{
			if (linked_ == nullptr)
				throw error_type::forbidden;
			return linked_->end();
		}

		numeric_value_property_type size;

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, string_value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			linked_ = linked;
			callback_ = callback;
			size.initialize_(nullptr, callback);
		}

		value_type *linked_;
		callback_type callback_;
	};

	template <class value_type, class manager_type = void>
	using read_only_string_value_property = string_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_string_value_property = string_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_STRING_PROPERTY_H */
