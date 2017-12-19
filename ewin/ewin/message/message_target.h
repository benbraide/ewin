#pragma once

#ifndef EWIN_MESSAGE_TARGET_H
#define EWIN_MESSAGE_TARGET_H

#include <functional>

#include "../common/transformation_property.h"

#include "../events/event_callback.h"
#include "../events/basic_event.h"

namespace ewin::window{
	class wnd_event;
}

namespace ewin::message{
	class target{
	public:
		typedef std::pair<common::types::result, common::types::result> result_pair_type;
		typedef std::function<void(events::message &, bool)> dispatch_callback_type;

		explicit target(window::wnd_event &events);

		virtual ~target();

		common::transformation_property<common::types::msg, common::types::result, target> dispatch_message;

	protected:
		virtual common::types::result dispatch_message_(common::types::msg &msg);

		template <typename event_type, typename return_type>
		common::types::result dispatch_message_to_(return_type(target::*callback)(event_type &), common::types::msg &msg,
			dispatch_callback_type dispatch_callback = nullptr){
			events::typed_callback<target, return_type, event_type> event_callback(*this, callback);
			event_type e(msg, event_callback, this);

			if (dispatch_callback != nullptr){
				dispatch_callback(e, true);
				if (!e.handled){
					auto result = (this->*callback)(e);
					if (!e.handled)
						e.result = result;
					dispatch_callback(e, false);
				}
			}
			else{//No dispatch callback
				auto result = (this->*callback)(e);
				if (!e.handled)
					e.result = result;
			}

			return e.result;
		}

		template <typename event_type>
		common::types::result dispatch_message_to_(void(target::*callback)(event_type &), common::types::msg &msg,
			dispatch_callback_type dispatch_callback = nullptr){
			events::typed_callback<target, void, event_type> event_callback(*this, callback);
			event_type e(msg, event_callback, this);

			if (dispatch_callback != nullptr){
				dispatch_callback(e, true);
				if (!e.handled){
					(this->*callback)(e);
					dispatch_callback(e, false);

					if (!e.handled)
						e.result = call_procedure_(e);
				}
			}
			else{//No dispatch callback
				(this->*callback)(e);
				if (!e.handled)
					e.result = call_procedure_(e);
			}

			return e.result;
		}

		template <typename event_type>
		common::types::result dispatch_message_to_(bool(target::*callback)(event_type &), common::types::msg &msg,
			dispatch_callback_type dispatch_callback = nullptr){
			events::typed_callback<target, bool, event_type> event_callback(*this, callback);
			event_type e(msg, event_callback, this);

			if (dispatch_callback != nullptr){
				dispatch_callback(e, true);
				if (!e.handled){
					if (!(this->*callback)(e) || e.handled)
						e.prevent_default = true;

					dispatch_callback(e, false);
					if (!e.handled)
						e.result = call_procedure_(e);
				}
			}
			else if ((this->*callback)(e) && !e.handled)
				e.result = call_procedure_(e);

			return e.result;
		}

		virtual bool on_pre_create_(events::message &e);

		virtual bool on_create_(events::message &e);

		virtual void on_destroy_(events::message &e);

		virtual void on_post_destroy_(events::message &e);

		virtual bool on_close_(events::message &e);

		virtual common::types::uint on_mouse_activate_(events::mouse_activate &e);

		virtual bool on_pre_activate_(events::pre_activate &e);

		virtual void on_activate_(events::activate &e);

		virtual void on_cancel_mode_(events::message &e);

		virtual void on_focus_change_(events::focus_change &e);

		virtual void on_enable_(events::enable &e);

		virtual void on_set_cursor_(events::cursor &e);

		virtual common::types::hcursor on_get_cursor_(events::cursor &e);

		virtual common::types::uint on_hit_test_(events::hit_test &e);

		virtual void on_position_change_(events::position_change &e);

		virtual void on_size_(events::size &e);

		virtual void on_move_(events::move &e);

		virtual void on_style_change_(events::move &e);

		virtual void on_unknown_message_(events::message &e);

		virtual common::types::result call_procedure_(events::message &e);

		common::types::procedure procedure_;
		window::wnd_event *events_;
	};
}

#endif /* !EWIN_MESSAGE_TARGET_H */
