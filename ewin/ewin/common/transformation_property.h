#pragma once

#ifndef EWIN_VALIDATION_PROPERTY_H
#define EWIN_VALIDATION_PROPERTY_H

#include <memory>

#include "value_property.h"

namespace ewin::common{
	template <class value_type, class return_type, class manager_type = void, property_access access = property_access::nil>
	class transformation_property{
	public:
		typedef value_type value_type;
		typedef return_type return_type;
		typedef manager_type manager_type;
		typedef property_access access_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;

		transformation_property(){}

		explicit transformation_property(callback_type callback)
			: callback_(callback){}

		template <typename unused_type = return_type>
		std::enable_if_t<std::is_void_v<unused_type>, return_type> operator [](const value_type &target) const{
			if (callback_ != nullptr)
				callback_(const_cast<transformation_property *>(this), &const_cast<value_type &>(target), access_type::read);
			else
				throw error_type::uninitialized_property;
		}

		template <typename unused_type = return_type>
		std::enable_if_t<!std::is_void_v<unused_type>, return_type> operator [](const value_type &target) const{
			if (callback_ == nullptr)
				throw error_type::uninitialized_property;

			auto info = std::make_pair(&const_cast<value_type &>(target), return_type());
			callback_(const_cast<transformation_property *>(this), &info, access_type::read);

			return info.second;
		}

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, transformation_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			callback_ = callback;
		}

		callback_type callback_;
	};
}

#endif /* !EWIN_VALIDATION_PROPERTY_H */
