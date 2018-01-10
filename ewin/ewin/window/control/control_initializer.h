#pragma once

#ifndef EWIN_CONTROL_INITIALIZER_H
#define EWIN_CONTROL_INITIALIZER_H

#include "../../common/macro.h"

#include "control_attributes.h"

namespace ewin::window::control{
	class initializer{
	public:
		template <id control_id>
		static common::types::procedure initialize(){
			if (!EWIN_IS(list_, attributes<control_id>::initializer)){
				common::types::init_common_controls info{
					sizeof(common::types::init_common_controls),
					attributes<control_id>::initializer
				};

				if (EWIN_CPP_BOOL(::InitCommonControlsEx(&info)))
					EWIN_SET(list_, attributes<control_id>::initializer);
			}

			common::types::wnd_class info;
			if (EWIN_CPP_BOOL(::GetClassInfoExW(nullptr, attributes<control_id>::class_name(), &info)))
				return info.lpfnWndProc;//Return control's procedure

			return nullptr;
		}

	private:
		static common::types::dword list_;
	};
}

#endif /* !EWIN_CONTROL_INITIALIZER_H */
