#pragma once

#ifndef EWIN_BUTTON_CONTROL_H
#define EWIN_BUTTON_CONTROL_H

#include "control_object.h"

namespace ewin::window::control{
	class button : public typed_object<id::button>{
	public:
		typedef typed_object<id::button> base_type;

		button();

		virtual ~button();

	protected:
		virtual void compute_size_() override;
	};
}

#endif /* !EWIN_BUTTON_CONTROL_H */
