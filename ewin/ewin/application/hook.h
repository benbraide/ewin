#pragma once

#ifndef EWIN_HOOK_H
#define EWIN_HOOK_H

#include <memory>
#include <functional>

#include "../common/common_headers.h"
#include "../common/boolean_property.h"

#include "../external/polyhook/polyhook.h"

namespace ewin::application{
	class hook{
	public:
		virtual ~hook() = default;
	};

	template <class function_type>
	class common_hook : public hook{
	public:
		virtual ~common_hook() = default;

		template <typename... args_types>
		auto call_original(args_types &&... args) const{
			return original_(std::forward<args_types>(args)...);
		}

		virtual function_type original() const{
			return original_;
		}

		virtual function_type hooked() const{
			return hooked_;
		}

	private:
		function_type original_;
		function_type hooked_;
	};

	template <class function_type>
	class detour_hook : public common_hook<function_type>{
	public:
		typedef function_type function_type;

		typedef PLH::X64Detour detour_type;
		typedef std::shared_ptr<detour_type> detour_ptr_type;

		detour_hook() = default;

		detour_hook(function_type original, function_type hooked){
			set(original, hooked);
		}

		detour_hook(const detour_hook &) = delete;
		
		detour_hook &operator =(const detour_hook &) = delete;

		virtual uint8_t *raw_original() const override{
			return reinterpret_cast<uint8_t *>(original());
		}

		void set(function_type original, function_type hooked){
			value_ = std::make_shared<detour_type>();
			value_->SetupHook(original, hooked_ = hooked);

			if (value_->Hook())
				original_ = value_->GetOriginal<function_type>();
			else//Failed to hook
				value_ = nullptr;
		}

		void remove(){
			value_ = nullptr;
		}

		bool is_hooked() const{
			return (value_ != nullptr);
		}

	private:
		detour_ptr_type value_;
	};

	template <class function_type>
	class iat_hook : public common_hook<function_type>{
	public:
		typedef function_type function_type;

		typedef PLH::IATHook detour_type;
		typedef std::shared_ptr<detour_type> detour_ptr_type;

		iat_hook(){}

		iat_hook(const char *dll, const char *name, function_type hooked){
			set(dll, name, hooked);
		}

		iat_hook(const iat_hook &) = delete;
		
		iat_hook &operator =(const iat_hook &) = delete;

		void set(const char *dll, const char *name, function_type hooked){
			value_ = std::make_shared<detour_type>();
			value_->SetupHook(dll, name, reinterpret_cast<uint8_t *>(hooked_ = hooked));
			if (value_->Hook())
				original_ = value_->GetOriginal<function_type>();
			else//Failed to hook
				value_ = nullptr;
		}

		void remove(){
			value_ = nullptr;
		}

		bool is_hooked() const{
			return (value_ != nullptr);
		}

	private:
		function_type original_;
		function_type hooked_;
		detour_ptr_type value_;
	};
}

#endif /* !EWIN_HOOK_H */
