#pragma once

#ifndef EWIN_WRITING_FONT_H
#define EWIN_WRITING_FONT_H

#include "writing_localized_strings.h"

namespace ewin::writing{
	class font{
	public:
		common::read_only_object_value_property<types::font, font> native;
		common::object_value_property<writing::factory, font> factory;
	};
}

#endif /* !EWIN_WRITING_FONT_H */
