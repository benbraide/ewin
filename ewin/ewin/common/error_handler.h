#pragma once

#ifndef EWIN_ERROR_HANDLER_H
#define EWIN_ERROR_HANDLER_H

#include "value_property.h"

namespace ewin::common{
	template <class value_type = error_type>
	class error_handler{
	public:
		typedef value_type value_type;

		struct property_forbidden_info{
			void *value;
			common::property_access access;
		};

		error_handler(){
			auto handler = std::bind(&error_handler::handle_error_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

			error_throw_policy.initialize_(nullptr, handler);
			error.initialize_(nullptr, handler);
		}

		value_property<error_throw_policy_type, error_handler> error_throw_policy;
		value_property<value_type, error_handler> error;

	protected:
		virtual void handle_error_property_(void *prop, void *arg, common::property_access access){
			if (prop == &error){
				if (access == common::property_access::read)
					*static_cast<error_type *>(arg) = error_value_;
				else if (access == common::property_access::write)
					set_error_(*static_cast<error_type *>(arg));
			}

			if (is_forbidden_(property_forbidden_info{ prop, access })){
				set_error_(error_type::forbidden_property);
				return;
			}

			error_value_ = error_type::nil;
			if (prop == &error_throw_policy){
				if (access == common::property_access::read)
					*static_cast<error_throw_policy_type *>(arg) = error_throw_policy_;
				else if (access == common::property_access::write)
					error_throw_policy_ = *static_cast<error_throw_policy_type *>(arg);
			}
		}

		virtual bool is_forbidden_(const property_forbidden_info &info){
			return false;
		}

		virtual void set_error_(error_type value){
			if (value == error_type::nil){//Clear error
				error_value_ = value;
				return;
			}

			switch (static_cast<error_throw_policy_type>(error_throw_policy_)){
			case error_throw_policy_type::never://Update last error
				error_value_ = value;
				break;
			case error_throw_policy_type::once://Throw once
				error_throw_policy_ = error_throw_policy_type::never;
				throw value;
				break;
			default:
				throw value;
				break;
			}
		}

		error_throw_policy_type error_throw_policy_;
		value_type error_value_;
	};
}

#endif /* !EWIN_ERROR_HANDLER_H */
