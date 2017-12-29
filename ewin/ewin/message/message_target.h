#pragma once

#ifndef EWIN_MESSAGE_TARGET_H
#define EWIN_MESSAGE_TARGET_H

#include <functional>

#include "../common/transformation_property.h"

#include "../events/event_callback.h"
#include "../events/basic_event.h"

#define EWIN_WM_WIN_FIRST		(EWIN_WM_APP_LAST  + 1)
#define EWIN_WM_POSITION_CHANGE	(EWIN_WM_WIN_FIRST + 0)
#define EWIN_WM_GET_CURSOR		(EWIN_WM_WIN_FIRST + 1)
#define EWIN_WM_GET_BG_BRUSH	(EWIN_WM_WIN_FIRST + 2)
#define EWIN_WM_GET_BG_COLOR	(EWIN_WM_WIN_FIRST + 3)
#define EWIN_WM_MOUSEENTER		(EWIN_WM_WIN_FIRST + 4)
#define EWIN_WM_MOUSELEAVE		(EWIN_WM_WIN_FIRST + 5)
#define EWIN_WM_MOUSEDRAG		(EWIN_WM_WIN_FIRST + 6)
#define EWIN_WM_MOUSEDRAGEND	(EWIN_WM_WIN_FIRST + 7)

namespace ewin::menu{
	class popup;
}

namespace ewin::window{
	class wnd_event;
}

namespace ewin::message{
	class target{
	public:
		typedef std::function<void(events::message &, bool)> dispatch_callback_type;
		typedef std::function<void(common::types::msg &)> bubble_callback_type;

		explicit target(window::wnd_event &events);

		virtual ~target();

		common::object_value_property<menu::popup, target> context_menu;
		common::transformation_property<common::types::msg, common::types::result, target> dispatch_message;

	protected:
		virtual common::types::result dispatch_message_(common::types::msg &msg, target *target);

		template <typename event_type, typename return_type>
		common::types::result dispatch_message_to_(return_type(target::*callback)(event_type &), common::types::msg &msg, target *target,
			dispatch_callback_type dispatch_callback = nullptr, bool *stop_propagation = nullptr){
			events::typed_callback<message::target, return_type, event_type> event_callback(*this, callback);
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
		common::types::result dispatch_message_to_(void(target::*callback)(event_type &), common::types::msg &msg, target *target,
			dispatch_callback_type dispatch_callback = nullptr, bool *stop_propagation = nullptr){
			events::typed_callback<message::target, void, event_type> event_callback(*this, callback);
			event_type e(msg, event_callback, target, this);

			if (dispatch_callback != nullptr){
				dispatch_callback(e, true);
				if (!e.handled){
					(this->*callback)(e);
					dispatch_callback(e, false);

					if (msg.message < WM_APP && !e.handled)
						e.result = call_procedure_(e);
				}
			}
			else{//No dispatch callback
				(this->*callback)(e);
				if (msg.message < WM_APP && !e.handled)
					e.result = call_procedure_(e);
			}

			if (stop_propagation != nullptr && e.stop_propagation)
				*stop_propagation = true;

			return e.result;
		}

		template <typename event_type>
		common::types::result dispatch_message_to_(bool(target::*callback)(event_type &), common::types::msg &msg, target *target,
			dispatch_callback_type dispatch_callback = nullptr, bool *stop_propagation = nullptr){
			events::typed_callback<message::target, bool, event_type> event_callback(*this, callback);
			event_type e(msg, event_callback, target, this);

			if (dispatch_callback != nullptr){
				dispatch_callback(e, true);
				if (!e.handled){
					if (!(this->*callback)(e) || e.handled)
						e.prevent_default = true;

					dispatch_callback(e, false);
					if (msg.message < WM_APP && !e.handled)
						e.result = call_procedure_(e);
				}
			}
			else if ((this->*callback)(e) && msg.message < WM_APP && !e.handled)
				e.result = call_procedure_(e);

			if (stop_propagation != nullptr && e.stop_propagation)
				*stop_propagation = true;

			return e.result;
		}

		template <typename event_type, typename return_type>
		common::types::result dispatch_bubbling_message_to_(return_type(target::*callback)(event_type &), common::types::msg &msg, target *target,
			dispatch_callback_type dispatch_callback = nullptr, bubble_callback_type bubble_callback = nullptr){
			auto parent = parent_();
			if (parent == nullptr)//Parent required for bubbling
				return dispatch_message_to_(callback, msg, target, dispatch_callback);

			auto stop_propagation = false;
			auto result = dispatch_message_to_(callback, msg, target, dispatch_callback, &stop_propagation);
			if ( stop_propagation)//Don't bubble
				return result;

			if (bubble_callback != nullptr)//Prepare for bubbling
				bubble_callback(msg);

			parent->dispatch_message_(msg, target);
			return result;
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

		virtual void on_style_change_(events::style_change &e);

		virtual void on_background_erase_(events::draw &e);

		virtual drawing::brush *on_get_background_brush_(events::message &e);

		virtual drawing::types::color *on_get_background_color_(events::message &e);

		virtual void on_non_client_paint_(events::draw &e);

		virtual void on_paint_(events::draw &e);

		virtual void on_print_client_(events::draw &e);

		virtual void on_mouse_leave_(events::mouse &e);

		virtual void on_mouse_enter_(events::mouse &e);

		virtual void on_mouse_move_(events::mouse &e);

		virtual void on_mouse_hover_(events::mouse &e);

		virtual void on_left_mouse_down_(events::mouse &e);

		virtual void on_middle_mouse_down_(events::mouse &e);

		virtual void on_right_mouse_down_(events::mouse &e);

		virtual void on_left_mouse_up_(events::mouse &e);

		virtual void on_middle_mouse_up_(events::mouse &e);

		virtual void on_right_mouse_up_(events::mouse &e);

		virtual void on_left_mouse_double_click_(events::mouse &e);

		virtual void on_middle_mouse_double_click_(events::mouse &e);

		virtual void on_right_mouse_double_click_(events::mouse &e);

		virtual void on_mouse_wheel_(events::mouse &e);

		virtual void on_mouse_drag_(events::mouse_drag &e);

		virtual void on_mouse_drag_end_(events::mouse &e);

		virtual void on_context_menu_(events::context_menu &e);

		virtual void on_key_down_(events::key_down &e);

		virtual void on_system_key_down_(events::key_down &e);

		virtual void on_key_up_(events::key &e);

		virtual void on_system_key_up_(events::key &e);

		virtual void on_key_press_(events::key_press &e);

		virtual void on_system_key_press_(events::key_press &e);

		virtual void on_dead_key_(events::key_press &e);

		virtual void on_dead_system_key_(events::key_press &e);

		virtual void on_unknown_message_(events::message &e);

		virtual common::types::result call_procedure_(events::message &e);

		virtual target *parent_() const = 0;

		common::types::procedure procedure_;
		menu::popup *context_menu_;
		window::wnd_event *events_;
		drawing::types::color background_color_;
	};
}

#endif /* !EWIN_MESSAGE_TARGET_H */
