#pragma once

#ifndef EWIN_MENU_MESSAGE_TARGET_H
#define EWIN_MENU_MESSAGE_TARGET_H

#include "../common/transformation_property.h"

#include "../events/event_callback.h"
#include "../events/basic_event.h"

#define EWIN_WM_MENU_CREATE			(EWIN_WM_MENU_FIRST  + 0)
#define EWIN_WM_MENU_DESTROY		(EWIN_WM_MENU_FIRST  + 1)
#define EWIN_WM_MENU_INIT			(EWIN_WM_MENU_FIRST  + 2)
#define EWIN_WM_MENU_HIGHLIGHT		(EWIN_WM_MENU_FIRST  + 3)
#define EWIN_WM_MENU_SELECT			(EWIN_WM_MENU_FIRST  + 4)
#define EWIN_WM_MENU_CHECK			(EWIN_WM_MENU_FIRST  + 5)

namespace ewin::menu{
	class menu_event;
}

namespace ewin::message{
	class menu_target{
	public:
		typedef std::function<void(events::menu_message &, bool)> dispatch_callback_type;
		typedef std::function<void(common::types::msg &)> bubble_callback_type;

		menu_target();

		virtual ~menu_target();

		common::transformation_property<common::types::msg, common::types::result, menu_target> dispatch_message;

	protected:
		virtual common::types::result dispatch_message_(common::types::msg &msg, menu_target *target);

		template <typename event_type, typename return_type>
		common::types::result dispatch_message_to_(return_type(menu_target::*callback)(event_type &), common::types::msg &msg, menu_target *target,
			dispatch_callback_type dispatch_callback = nullptr, bool *stop_propagation = nullptr){
			events::typed_callback<menu_target, return_type, event_type> event_callback(*this, callback);
			event_type e(msg, event_callback, target, this);

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

			if (stop_propagation != nullptr && e.stop_propagation)
				*stop_propagation = true;

			return e.result;
		}

		template <typename event_type>
		common::types::result dispatch_message_to_(void(menu_target::*callback)(event_type &), common::types::msg &msg, menu_target *target,
			dispatch_callback_type dispatch_callback = nullptr, bool *stop_propagation = nullptr){
			events::typed_callback<menu_target, void, event_type> event_callback(*this, callback);
			event_type e(msg, event_callback, target, this);

			if (dispatch_callback != nullptr){
				dispatch_callback(e, true);
				if (!e.handled){
					(this->*callback)(e);
					dispatch_callback(e, false);
				}
			}
			else//No dispatch callback
				(this->*callback)(e);

			if (!e.handled)//Prevent default handling
				e.result = 0;

			if (stop_propagation != nullptr && e.stop_propagation)
				*stop_propagation = true;

			return e.result;
		}

		template <typename event_type>
		common::types::result dispatch_message_to_(bool(menu_target::*callback)(event_type &), common::types::msg &msg, menu_target *target,
			dispatch_callback_type dispatch_callback = nullptr, bool *stop_propagation = nullptr){
			events::typed_callback<menu_target, bool, event_type> event_callback(*this, callback);
			event_type e(msg, event_callback, target, this);

			if (dispatch_callback != nullptr){
				dispatch_callback(e, true);
				if (!e.handled && (this->*callback)(e) && !e.handled)
					dispatch_callback(e, false);
			}
			else//No dispatch callback
				(this->*callback)(e);

			if (!e.handled)//Prevent default handling
				e.result = 0;

			if (stop_propagation != nullptr && e.stop_propagation)
				*stop_propagation = true;

			return e.result;
		}

		template <typename event_type, typename return_type>
		common::types::result dispatch_bubbling_message_to_(return_type(menu_target::*callback)(event_type &), common::types::msg &msg, menu_target *target,
			dispatch_callback_type dispatch_callback = nullptr, bubble_callback_type bubble_callback = nullptr){
			auto parent = parent_();
			if (parent == nullptr)//Parent required for bubbling
				return dispatch_message_to_(callback, msg, target, dispatch_callback);

			auto stop_propagation = false;
			auto result = dispatch_message_to_(callback, msg, target, dispatch_callback, &stop_propagation);
			if (stop_propagation)//Don't bubble
				return result;

			if (bubble_callback != nullptr)//Prepare for bubbling
				bubble_callback(msg);

			parent->dispatch_message_(msg, target);
			return result;
		}

		virtual void on_create_(ewin::events::menu_message &e);

		virtual void on_destroy_(ewin::events::menu_message &e);

		virtual void on_highlight_(ewin::events::menu_message &e);

		virtual void on_select_(ewin::events::menu_message &e);

		virtual void on_check_(ewin::events::menu_message &e);

		virtual bool on_init_(ewin::events::menu_message &e);

		virtual void on_unknown_message_(ewin::events::menu_message &e);

		virtual menu_target *parent_() const = 0;

		virtual ewin::menu::menu_event *get_events_() = 0;

		virtual bool has_events_() const = 0;
	};
}

#endif /* !EWIN_MENU_MESSAGE_TARGET_H */
