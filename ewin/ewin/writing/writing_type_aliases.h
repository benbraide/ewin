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
	typedef ::IDWriteFontFamily					font_family;
	typedef ::IDWriteFontCollection				font_collection;

	typedef ::DWRITE_FONT_WEIGHT				font_weight;
	typedef ::DWRITE_FONT_STRETCH				font_stretch;
	typedef ::DWRITE_FONT_STYLE					font_style;

	typedef ::IDWriteTextFormat					text_format;
	typedef ::IDWriteTextLayout					text_layout;

	typedef ::DWRITE_TEXT_METRICS				text_metrics;
}

#endif /* !EWIN_WRITING_TYPE_ALIASES_H */
