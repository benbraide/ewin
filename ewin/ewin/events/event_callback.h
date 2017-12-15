#pragma once

#ifndef EWIN_EVENT_CALLBACK_H
#define EWIN_EVENT_CALLBACK_H

#include "event_object.h"

namespace ewin::events{
	class callback{
	public:
		virtual ~callback() = default;

		virtual common::types::result operator()(object &e) = 0;
	};

	template <class target_type, class return_type, class parameter_type>
	class typed_callback : public callback{
	public:
		typedef target_type target_type;
		typedef return_type return_type;
		typedef parameter_type parameter_type;

		typedef return_type(target_type::*callback_type)(parameter_type &);
		typedef std::function<common::types::result(const std::conditional_t<std::is_void_v<return_type>, void *, return_type> &)> converter_type;

		explicit typed_callback(callback_type callback, converter_type converter = nullptr)
			: callback_(callback), converter_(converter){}

		virtual common::types::result operator()(object &e) override{
			return call_(e);
		}

	protected:
		template <typename unused_type = return_type>
		std::enable_if<std::is_void_v<unused_type>, common::types::result> call_(object &e){
			callback_(*dynamic_cast<parameter_type *>(&e));
			return 0u;
		}

		template <typename unused_type = return_type>
		std::enable_if<std::is_same_v<unused_type, bool>, common::types::result> call_(object &e){
			if (converter_ == nullptr)
				return EWIN_C_BOOL(callback_(*dynamic_cast<parameter_type *>(&e)));
			return converter_(callback_(*dynamic_cast<parameter_type *>(&e)));
		}

		template <typename unused_type = return_type>
		std::enable_if<!std::is_void_v<unused_type> && !std::is_same_v<unused_type, bool>, common::types::result> call_(object &e){
			if (converter_ == nullptr)
				return (return_type)callback_(*dynamic_cast<parameter_type *>(&e));
			return converter_(callback_(*dynamic_cast<parameter_type *>(&e)));
		}

		callback_type callback_;
		converter_type converter_;
	};
}

#endif /* !EWIN_EVENT_CALLBACK_H */
