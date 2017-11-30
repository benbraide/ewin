#pragma once

#ifndef EWIN_VALUE_PROPERTY_H
#define EWIN_VALUE_PROPERTY_H

#include <functional>

#include "macro.h"

namespace ewin::common{
	enum class property_error{
		nil,
		uninitialized,
		access_violation,
		forbidden,
	};

	enum class property_access : unsigned int{
		nil					= (0 << 0x0000),
		read				= (1 << 0x0000),
		write				= (1 << 0x0001),
		list_add			= (1 << 0x0002),
		list_remove			= (1 << 0x0003),
		list_at				= (1 << 0x0004),
		list_find			= (1 << 0x0005),
		list_begin			= (1 << 0x0006),
		list_end			= (1 << 0x0007),
		write_alt			= (1 << 0x0008),
	};

	class property_manager{
	public:
		virtual ~property_manager() = default;

	protected:
		template <typename property_type, typename value_type, typename callback_type>
		void initialize_(property_type &prop, value_type *linked, callback_type callback){
			prop.initialize_(linked, callback);
		}
	};

	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class value_property{
	public:
		typedef value_type value_type;
		typedef manager_type manager_type;

		typedef property_error error_type;
		typedef property_access access_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;

		value_property()
			: linked_(nullptr){}

		explicit value_property(value_type &linked)
			: linked_(&linked){}

		explicit value_property(callback_type callback)
			: linked_(nullptr), callback_(callback){}

		value_property(value_type &linked, callback_type callback)
			: linked_(&linked), callback_(callback){}

		template <typename target_type>
		value_property &operator =(const target_type &value){
			return operator =((value_type)value);
		}

		value_property &operator =(const value_type &value){
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
					callback_(const_cast<value_property *>(this), nullptr, access_type::read);
				return *linked_;
			}

			if (callback_ != nullptr){//Call handler
				auto value = value_type();
				callback_(const_cast<value_property *>(this), &value, access_type::read);
				return value;
			}

			throw error_type::uninitialized;//Error
		}

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			linked_ = linked;
			callback_ = callback;
		}

		value_type *linked_;
		callback_type callback_;
	};

	template <class value_type, class manager_type = void>
	using read_only_value_property = value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_value_property = value_property<value_type, manager_type, property_access::write>;

	EWIN_MAKE_OPERATORS(property_access)
}

#endif /* !EWIN_VALUE_PROPERTY_H */
