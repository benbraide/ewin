#pragma once

#ifndef EWIN_LIST_PROPERTY_H
#define EWIN_LIST_PROPERTY_H

#include <vector>
#include <memory>

#include "numeric_property.h"

namespace ewin::common{
	template <class value_type, class iterator_type, class const_iterator_type, class manager_type = void, property_access access = property_access::nil>
	class list_value_property{
	public:
		typedef value_type value_type;
		typedef iterator_type iterator_type;
		typedef const_iterator_type const_iterator_type;
		typedef manager_type manager_type;
		typedef property_access access_type;

		typedef std::vector<value_type> list_type;
		typedef std::function<void(void *, void *, access_type)> callback_type;

		typedef numeric_value_property<std::size_t, list_value_property, access_type::read> numeric_value_property_type;

		list_value_property(){}

		explicit list_value_property(callback_type callback)
			: callback_(callback){}

		list_value_property &operator +=(const value_type &value){
			if (access != access_type::nil && !EWIN_IS(access, access_type::list_add))
				throw error_type::property_access_violation;

			if (callback_ != nullptr)//Call handler
				callback_(this, &const_cast<value_type &>(value), access_type::list_add);
			else//Error
				throw error_type::uninitialized_property;

			return *this;
		}

		list_value_property &operator -=(const value_type &value){
			if (access != access_type::nil && !EWIN_IS(access, access_type::list_remove))
				throw error_type::property_access_violation;

			if (callback_ != nullptr)//Call handler
				callback_(this, &const_cast<value_type &>(value), access_type::list_remove);
			else//Error
				throw error_type::uninitialized_property;

			return *this;
		}

		value_type *operator [](std::size_t index) const{
			if (access != access_type::nil && !EWIN_IS(access, access_type::list_at))
				throw error_type::property_access_violation;

			auto info = std::make_pair<std::size_t, value_type *>(index, nullptr);
			if (callback_ != nullptr)//Call handler
				callback_(const_cast<list_value_property *>(this), &info, access_type::list_at);
			else//Error
				throw error_type::uninitialized_property;

			return info.second;
		}

		std::size_t operator [](const value_type &value) const{
			if (access != access_type::nil && !EWIN_IS(access, access_type::list_find))
				throw error_type::property_access_violation;

			auto info = std::make_pair<std::size_t, value_type *>(static_cast<std::size_t>(-1), &const_cast<value_type &>(value));
			if (callback_ != nullptr)//Call handler
				callback_(const_cast<list_value_property *>(this), &info, access_type::list_find);
			else//Error
				throw error_type::uninitialized_property;

			return info.first;
		}

		operator bool() const{
			return (size > 0u);
		}

		iterator_type begin(){
			if (callback_ == nullptr)
				throw error_type::uninitialized_property;

			auto value = iterator_type();
			callback_(this, &value, access_type::list_begin);
			return value;
		}

		const_iterator_type begin() const{
			return const_cast<list_value_property *>(this)->begin();
		}

		iterator_type end(){
			if (callback_ == nullptr)
				throw error_type::uninitialized_property;

			auto value = iterator_type();
			callback_(this, &value, access_type::list_end);
			return value;
		}

		const_iterator_type end() const{
			return const_cast<list_value_property *>(this)->end();
		}

		numeric_value_property_type size;

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, list_value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			callback_ = callback;
			size.initialize_(nullptr, callback);
		}

		callback_type callback_;
	};

	template <class value_type, class iterator_type, class const_iterator_type, class manager_type = void>
	using read_only_list_value_property = list_value_property<value_type, iterator_type, const_iterator_type, manager_type, property_access::read>;

	template <class value_type, class iterator_type, class const_iterator_type, class manager_type = void>
	using write_only_list_value_property = list_value_property<value_type, iterator_type, const_iterator_type, manager_type, property_access::write>;
}

#endif /* !EWIN_LIST_PROPERTY_H */
