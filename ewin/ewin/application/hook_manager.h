#pragma once

#ifndef EWIN_HOOK_MANAGER_H
#define EWIN_HOOK_MANAGER_H

#include <list>
#include <unordered_map>

#include "../common/type_aliases.h"

#include "hook.h"

namespace ewin::application{
	class hook_manager{
	public:
		typedef std::shared_ptr<hook> hook_ptr_type;

		struct iat_info{
			const char *dll;
			const char *name;
			hook_ptr_type hook;
		};

		typedef std::unordered_map<common::types::uint, hook_ptr_type> hook_list_type;

		template <typename function_type, typename hooked_function_type>
		void add(common::types::uint id, function_type original, hooked_function_type hooked){
			hook_list_[id] = std::make_shared<detour_hook<function_type>>(original, hooked);
		}

		template <typename function_type, typename hooked_function_type>
		void add(common::types::uint id, const char *dll, const char *name, hooked_function_type hooked){
			hook_list_[id] = std::make_shared<iat_hook<function_type>>(dll, name, hooked);
		}

		template <typename function_type, typename... args_types>
		auto call_original(common::types::uint id, args_types &&... args) const{
			auto entry = hook_list_.find(id);
			return dynamic_cast<common_hook<function_type> *>(entry->second.get())->call_original(std::forward<args_types>(args)...);
		}

		template <typename function_type>
		function_type original(common::types::uint id) const{
			return dynamic_cast<common_hook<function_type> *>(hook_list_[id].get())->original();
		}

	private:
		hook_list_type hook_list_;
	};
}

#endif /* !EWIN_HOOK_MANAGER_H */
