#pragma once

#ifndef EWIN_EVENT_CALLBACK_H
#define EWIN_EVENT_CALLBACK_H

#include "event_object.h"

namespace ewin::events{
	template <class target_type>
	class basic_callback{
	public:
		typedef target_type target_type;

		virtual ~basic_callback() = default;

		virtual common::types::result operator()(basic_object<target_type> &e) = 0;
	};

	template <class target_type, class return_type, class parameter_type>
	class typed_callback : public basic_callback<typename message_target_from_object<target_type>::type>{
	public:
		typedef basic_callback<typename message_target_from_object<target_type>::type> base_type;
		typedef typename base_type::target_type base_target_type;

		typedef target_type target_type;
		typedef return_type return_type;
		typedef parameter_type parameter_type;

		typedef return_type(target_type::*callback_type)(parameter_type &);
		typedef std::function<common::types::result(const std::conditional_t<std::is_void_v<return_type>, void *, return_type> &)> converter_type;

		explicit typed_callback(target_type &target, callback_type callback, converter_type converter = nullptr)
			: target_(target), callback_(callback), converter_(converter){}

		virtual common::types::result operator()(basic_object<base_target_type> &e) override{
			return call_(e);
		}

	protected:
		template <typename unused_type = return_type>
		std::enable_if_t<std::is_void_v<unused_type>, common::types::result> call_(basic_object<base_target_type> &e){
			(target_.*callback_)(*dynamic_cast<parameter_type *>(&e));
			return 0u;
		}

		template <typename unused_type = return_type>
		std::enable_if_t<std::is_same_v<unused_type, bool>, common::types::result> call_(basic_object<base_target_type> &e){
			if (converter_ == nullptr)
				return EWIN_C_BOOL((target_.*callback_)(*dynamic_cast<parameter_type *>(&e)));
			return converter_((target_.*callback_)(*dynamic_cast<parameter_type *>(&e)));
		}

		template <typename unused_type = return_type>
		std::enable_if_t<!std::is_void_v<unused_type> && !std::is_same_v<unused_type, bool>, common::types::result> call_(basic_object<base_target_type> &e){
			if (converter_ == nullptr)
				return (common::types::result)(target_.*callback_)(*dynamic_cast<parameter_type *>(&e));
			return converter_((target_.*callback_)(*dynamic_cast<parameter_type *>(&e)));
		}

		target_type &target_;
		callback_type callback_;
		converter_type converter_;
	};

	using callback = basic_callback<ewin::message::target>;
	using menu_callback = basic_callback<ewin::message::menu_target>;
}

#endif /* !EWIN_EVENT_CALLBACK_H */
