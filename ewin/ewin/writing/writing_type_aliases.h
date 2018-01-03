#pragma once

#ifndef EWIN_WRITING_TYPE_ALIASES_H
#define EWIN_WRITING_TYPE_ALIASES_H

#include "../common/common_headers.h"

namespace ewin::writing::types{
	typedef ::HFONT								gdi_font;
	typedef ::LOGFONTW							gdi_font_info;

	typedef ::HRESULT							result;
	typedef ::IUnknown							unknown;

	typedef ::DWRITE_FACTORY_TYPE				factory_type;
	typedef ::IDWriteFactory					factory;
	typedef ::IDWriteLocalizedStrings			localized_strings;

	typedef ::IDWriteFont						font;
}

#endif /* !EWIN_WRITING_TYPE_ALIASES_H */
