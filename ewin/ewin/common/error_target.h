#pragma once

#ifndef EWIN_ERROR_TARGET_H
#define EWIN_ERROR_TARGET_H

#include <variant>

#include "type_aliases.h"
#include "variant_property.h"

namespace ewin::common{
	template <class local_error_type = types::dword, local_error_type success_value = local_error_type(0)>
	class error_target{
	public:
		typedef local_error_type local_error_type;

		error_target()
			: error_value_(error_type::nil){
			error_throw_policy.initialize_(nullptr, [this](void *prop, void *arg, property_access access){
				if (access == common::property_access::read)
					*static_cast<error_throw_policy_type *>(arg) = error_throw_policy_;
				else if (access == common::property_access::write)
					error_throw_policy_ = *static_cast<error_throw_policy_type *>(arg);
			});

			error.initialize_(nullptr, [this](void *prop, void *arg, property_access access){
				if (access == property_access::read){
					auto &info = *static_cast<variant_value_property_arg_info *>(arg);
					switch (info.index){
					case 0u://Retrieve error value
						if (std::holds_alternative<error_type>(error_value_))
							*static_cast<error_type *>(info.value) = std::get<error_type>(error_value_);
						else//Holds local error value
							*static_cast<error_type *>(info.value) = error_type::local_error;
						break;
					case 1u://Retrieve local error value
						if (std::holds_alternative<local_error_type>(error_value_))
							*static_cast<local_error_type *>(info.value) = std::get<local_error_type>(error_value_);
						else//Holds error value
							*static_cast<local_error_type *>(info.value) = success_value;
						break;
					default:
						break;
					}
				}
				else if (access == property_access::write)
					set_error_(*static_cast<variant_value_property_arg_info *>(arg));
			});
		}

		virtual ~error_target() = default;

		common::value_property<error_throw_policy_type, error_target> error_throw_policy;
		common::variant_value_property<error_target, common::property_access::nil, error_type, local_error_type> error;

		static const local_error_type success_value = success_value;

	protected:
		virtual void set_error_(variant_value_property_arg_info &info){
			switch (info.index){
			case 0u:
				set_error_(*static_cast<error_type *>(info.value));
				break;
			case 1u:
				set_error_(*static_cast<local_error_type *>(info.value));
				break;
			default:
				break;
			}
		}

		virtual void set_error_(error_type value){
			if (value == error_type::nil){//Clear error
				error_value_ = value;
				return;
			}

			error_value_ = value;//Update last error
			switch (static_cast<error_throw_policy_type>(error_throw_policy_)){
			case error_throw_policy_type::no_throw_once:
				error_throw_policy_ = error_throw_policy_type::always;
				break;
			case error_throw_policy_type::never:
				break;
			case error_throw_policy_type::once://Throw once
				error_throw_policy_ = error_throw_policy_type::never;
				throw value;
				break;
			default://Always throw
				throw value;
				break;
			}
		}

		virtual void set_error_(local_error_type value){
			if (value == success_value){//Clear error
				error_value_ = error_type::nil;
				return;
			}

			error_value_ = value;//Update last error
			switch (static_cast<error_throw_policy_type>(error_throw_policy_)){
			case error_throw_policy_type::no_throw_once:
				error_throw_policy_ = error_throw_policy_type::always;
				break;
			case error_throw_policy_type::never:
				break;
			case error_throw_policy_type::once://Throw once
				error_throw_policy_ = error_throw_policy_type::never;
				throw error_type::local_error;
				break;
			default://Always throw
				throw error_type::local_error;
				break;
			}
		}

		error_throw_policy_type error_throw_policy_;
		std::variant<error_type, local_error_type> error_value_;
	};
}

#endif /* !EWIN_ERROR_TARGET_H */
