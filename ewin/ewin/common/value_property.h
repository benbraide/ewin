#pragma once

#ifndef EWIN_VALUE_PROPERTY_H
#define EWIN_VALUE_PROPERTY_H

#include <functional>

#include "macro.h"
#include "error.h"

#define EWIN_PROP_HANDLER(t) std::bind(&t::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

#define EWIN_PROP_FRIEND_OPTYPE(t, r) std::enable_if_t<!std::is_same_v<target_type, t<unused_type, manager_type, access>>, r>
#define EWIN_SPEC_PROP_FRIEND_OPTYPE(t, r) std::enable_if_t<!std::is_same_v<target_type, t<unused_type, access>>, r>
#define EWIN_EX_PROP_FRIEND_OPTYPE(t, r, e) std::enable_if_t<!std::is_same_v<target_type, t<unused_type, manager_type, access, e>>, r>

#define EWIN_PROP_FRIEND_OPCOMP(t)																\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, bool) operator ==(const target_type &lhs, const t &rhs){		\
	return (rhs == lhs);																		\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_PROP_FRIEND_OPTYPE(t, bool) operator !=(const target_type &lhs, const t &rhs){		\
	return (rhs != lhs);																		\
}

#define EWIN_SPEC_PROP_FRIEND_OPCOMP(t)															\
template <typename target_type, typename unused_type = manager_type>							\
friend EWIN_SPEC_PROP_FRIEND_OPTYPE(t, bool) operator ==(const target_type &lhs, const t &rhs){	\
	return (rhs == lhs);																		\
}																								\
																								\
template <typename target_type, typename unused_type = manager_type>							\
friend EWIN_SPEC_PROP_FRIEND_OPTYPE(t, bool) operator !=(const target_type &lhs, const t &rhs){	\
	return (rhs != lhs);																		\
}

#define EWIN_EX_PROP_FRIEND_OPCOMP(t, e)														\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_EX_PROP_FRIEND_OPTYPE(t, bool, e) operator ==(const target_type &lhs, const t &rhs){\
	return (rhs == lhs);																		\
}																								\
																								\
template <typename target_type, typename unused_type = value_type>								\
friend EWIN_EX_PROP_FRIEND_OPTYPE(t, bool, e) operator !=(const target_type &lhs, const t &rhs){\
	return (rhs != lhs);																		\
}

namespace ewin::common{
	enum class property_access : unsigned int{
		nil					= (0 << 0x0000),
		read				= (1 << 0x0000),
		write				= (1 << 0x0001),
		list_add			= (1 << 0x0002),
		list_remove			= (1 << 0x0003),
		list_remove_index	= (1 << 0x0004),
		list_at				= (1 << 0x0005),
		list_find			= (1 << 0x0006),
		list_begin			= (1 << 0x0007),
		list_end			= (1 << 0x0008),
		list_size			= (1 << 0x0009),
		write_alt			= (1 << 0x000A),
		increment			= (1 << 0x000B),
		decrement			= (1 << 0x000C),
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

	class property_object{
	public:
		property_object() = default;

		property_object(const property_object &) = delete;
		
		property_object &operator =(const property_object &) = delete;
	};

	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class value_property : public property_object{
	public:
		typedef value_type value_type;
		typedef manager_type manager_type;
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

		value_property &operator =(const value_property &value){
			return operator =((value_type)value);
		}

		value_property &operator =(const value_type &value){
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
					callback_(const_cast<value_property *>(this), nullptr, access_type::read);
				return *linked_;
			}

			if (callback_ != nullptr){//Call handler
				auto value = value_type();
				callback_(const_cast<value_property *>(this), &value, access_type::read);
				return value;
			}

			throw error_type::uninitialized_property;//Error
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
