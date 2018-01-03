#pragma once

#ifndef EWIN_WRITING_LOCALIZED_STRINGS_H
#define EWIN_WRITING_LOCALIZED_STRINGS_H

#include "../common/numeric_property.h"
#include "../common/transformation_property.h"

#include "writing_factory.h"

namespace ewin::writing{
	class localized_strings : public shared_com_native<types::localized_strings>{
	public:
		typedef shared_com_native<types::localized_strings> base_type;

		template <typename... args_types>
		explicit localized_strings(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			bind_properties_();
		}

		std::wstring operator [](common::types::uint index) const;

		std::wstring operator [](const wchar_t *locale) const;

		std::wstring operator [](const std::wstring &locale) const;

		common::read_only_numeric_value_property<common::types::uint, localized_strings> count;

	protected:
		void bind_properties_();
	};
}

#endif /* !EWIN_WRITING_LOCALIZED_STRINGS_H */
