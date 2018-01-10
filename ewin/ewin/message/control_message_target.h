#pragma once

#ifndef EWIN_CONTROL_MESSAGE_TARGET_H
#define EWIN_CONTROL_MESSAGE_TARGET_H

#include "message_target.h"

namespace ewin::message{
	class control_target : public target{
	public:
		virtual ~control_target();
	};
}

#endif /* !EWIN_CONTROL_MESSAGE_TARGET_H */
