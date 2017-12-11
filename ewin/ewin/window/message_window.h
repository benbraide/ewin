#pragma once

#ifndef EWIN_MESSAGE_WINDOW_H
#define EWIN_MESSAGE_WINDOW_H

#include "window_object.h"

namespace ewin::window{
	class message : public object{
	public:
		virtual ~message();

	protected:
		virtual bool is_forbidden_(const property_forbidden_info &info);

		virtual void create_(bool create, const create_info *info);

		virtual common::types::uint white_listed_styles_(bool is_extended) const;

		virtual common::types::uint black_listed_styles_(bool is_extended) const;
	};
}

#endif /* !EWIN_MESSAGE_WINDOW_H */
