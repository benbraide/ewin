#pragma once

#ifndef EWIN_UP_DOWN_PROPERTY_H
#define EWIN_UP_DOWN_PROPERTY_H

#include "value_property.h"

namespace ewin::common{
	template <class manager_type = void, property_access access = property_access::nil>
	class up_down_property : public property_object{
	public:
		typedef manager_type manager_type;
		typedef property_access access_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;

		up_down_property() = default;

		explicit up_down_property(callback_type callback)
			: callback_(callback){}

		up_down_property &operator ++(){
			if (access != access_type::nil && !EWIN_IS(access, access_type::increment))
				throw error_type::property_access_violation;

			if (callback_ != nullptr)
				callback_(this, nullptr, access_type::increment);
			else//Error
				throw error_type::uninitialized_property;
		}

		up_down_property &operator ++(int){
			return ++(*this);
		}

		up_down_property &operator --(){
			if (access != access_type::nil && !EWIN_IS(access, access_type::decrement))
				throw error_type::property_access_violation;

			if (callback_ != nullptr)
				callback_(this, nullptr, access_type::decrement);
			else//Error
				throw error_type::uninitialized_property;
		}

		up_down_property &operator --(int){
			return ++(*this);
		}

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, up_down_property, manager_type>;

		void initialize_(void *linked, callback_type callback){
			callback_ = callback;
		}

		callback_type callback_;
	};

	template <class manager_type = void>
	using increment_only_up_down_property = up_down_property<manager_type, property_access::increment>;

	template <class manager_type = void>
	using decrement_only_up_down_property = up_down_property<manager_type, property_access::decrement>;
}

#endif /* !EWIN_UP_DOWN_PROPERTY_H */
