#pragma once

#ifndef EWIN_VARIANT_PROPERTY_H
#define EWIN_VARIANT_PROPERTY_H

#include "variadic_type_index.h"
#include "value_property.h"

namespace ewin::common{
	template <class manager_type, property_access access, class... type_list>
	class variant_value_property{
	public:
		typedef void value_type;

		typedef manager_type manager_type;
		typedef property_access access_type;

		typedef std::function<void(void *, void *, access_type)> callback_type;

		struct arg_info{
			void *value;
			std::size_t index;
		};

		variant_value_property(){}

		explicit variant_value_property(callback_type callback)
			: callback_(callback){}

		template <typename target_type>
		variant_value_property &operator =(const target_type &value){
			if (access != access_type::nil && !EWIN_IS(access, access_type::write))
				throw error_type::property_access_violation;

			if (callback_ == nullptr)//Error
				throw error_type::uninitialized_property;

			arg_info info{ &const_cast<target_type &>(value), variadic_type_index<target_type, type_list...>::value };
			callback_(this, &info, access_type::write);

			return *this;
		}

		template <typename target_type>
		operator target_type() const{
			if (access != access_type::nil && !EWIN_IS(access, access_type::read))
				throw error_type::property_access_violation;

			if (callback_ == nullptr)//Error
				throw error_type::uninitialized_property;

			auto value = target_type();
			arg_info info{ &value, variadic_type_index<target_type, type_list...>::value };

			callback_(const_cast<variant_value_property *>(this), &info, access_type::read);
			return value;
		}

		static const property_access required_access = access;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, variant_value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			callback_ = callback;
		}

		callback_type callback_;
	};

	template <class manager_type, class... type_list>
	using read_only_variant_value_property = variant_value_property<manager_type, property_access::read, type_list...>;

	template <class manager_type, class... type_list>
	using write_only_variant_value_property = variant_value_property<manager_type, property_access::write, type_list...>;
}

#endif /* !EWIN_VARIANT_PROPERTY_H */
