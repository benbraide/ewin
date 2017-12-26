#pragma once

#ifndef EWIN_OBJECT_PROPERTY_H
#define EWIN_OBJECT_PROPERTY_H

#include "value_property.h"

namespace ewin::common{
	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class object_value_property : public property_object{
	public:
		typedef value_type value_type;
		typedef manager_type manager_type;
		typedef property_access access_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;

		object_value_property()
			: linked_(nullptr){}

		explicit object_value_property(value_type &linked)
			: linked_(&linked){}

		explicit object_value_property(callback_type callback)
			: linked_(nullptr), callback_(callback){}

		object_value_property(value_type &linked, callback_type callback)
			: linked_(&linked), callback_(callback){}

		template <typename target_type, typename unused_type = value_type>
		std::enable_if_t<std::is_base_of_v<property_object, target_type>, object_value_property<unused_type, manager_type, access>> &operator =(const target_type &value){
			return operator =((value_type *)value);
		}

		template <typename target_type, typename unused_type = value_type>
		std::enable_if_t<!std::is_base_of_v<property_object, target_type>, object_value_property<unused_type, manager_type, access>> &operator =(const target_type &value){
			return operator =((const value_type &)value);
		}

		template <typename target_type>
		object_value_property &operator =(const target_type *value){
			return operator =((value_type *)const_cast<target_type *>(value));
		}

		object_value_property &operator =(const object_value_property &value){
			return operator =((value_type *)value);
		}

		object_value_property &operator =(const value_type *value){
			if (access != access_type::nil && !EWIN_IS(access, access_type::write))
				throw error_type::property_access_violation;

			if (callback_ != nullptr)//Call handler
				callback_(this, const_cast<value_type *>(value), access_type::write);
			else//Error
				throw error_type::uninitialized_property;

			return *this;
		}

		object_value_property &operator =(const value_type &value){
			return operator =(&value);
		}

		operator value_type *() const{
			if (access != access_type::nil && !EWIN_IS(access, access_type::read))
				throw error_type::property_access_violation;

			if (linked_ != nullptr){
				if (callback_ != nullptr)//Alert listener
					callback_(const_cast<object_value_property *>(this), nullptr, access_type::read);
				return linked_;
			}

			if (callback_ != nullptr){//Call handler
				value_type *value = nullptr;
				callback_(const_cast<object_value_property *>(this), &value, access_type::read);
				return value;
			}

			throw error_type::uninitialized_property;//Error
		}

		value_type *operator ->() const{
			return operator value_type *();
		}

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, object_value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			linked_ = linked;
			callback_ = callback;
		}

		value_type *linked_;
		callback_type callback_;
	};

	template <class value_type, class manager_type = void>
	using read_only_object_value_property = object_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_object_value_property = object_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_OBJECT_PROPERTY_H */
