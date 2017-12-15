#pragma once

#ifndef EWIN_MESSAGE_TARGET_H
#define EWIN_MESSAGE_TARGET_H

#include "../events/event_object.h"

namespace ewin::message{
	class target{
	public:
		virtual ~target();

	protected:
		virtual bool on_pre_create_(events::message &e);

		virtual bool on_create_(events::message &e);

		virtual void on_destroy_(events::message &e);

		virtual void on_post_destroy_(events::message &e);

		virtual bool on_close_(events::message &e);

		virtual common::types::uint on_mouse_activate_(events::mouse_activate &e);

		virtual common::types::hcursor on_set_cursor_(events::set_cursor &e);
	};
}

#endif /* !EWIN_MESSAGE_TARGET_H */
