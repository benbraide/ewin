#pragma once

#ifndef EWIN_WRITING_TYPE_ALIASES_H
#define EWIN_WRITING_TYPE_ALIASES_H

#include "../common/common_headers.h"

namespace ewin::writing::types{
	typedef ::HRESULT							result;
	typedef ::IUnknown							unknown;

	typedef ::DWRITE_FACTORY_TYPE				factory_type;
	typedef ::IDWriteFactory					factory;
}

#endif /* !EWIN_WRITING_TYPE_ALIASES_H */
