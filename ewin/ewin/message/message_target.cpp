#include "../window/window_object.h"

ewin::message::target::target()
	: procedure_(::DefWindowProcW), context_menu_(nullptr){
	dispatch_message.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		auto info = static_cast<std::pair<common::types::msg *, common::types::result> *>(arg);
		info->second = dispatch_message_(*info->first, nullptr);
	});

	context_menu.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		if (access == common::property_access::read)
			*static_cast<menu::popup **>(arg) = context_menu_;
		else if (access == common::property_access::write)
			context_menu_ = static_cast<menu::popup *>(arg);
	});

	background_color_ = drawing::d2d1::ColorF(::GetSysColor(COLOR_WINDOW));
}

ewin::message::target::~target() = default;

ewin::common::types::result ewin::message::target::dispatch_message_(common::types::msg &msg, target *target){
	switch (msg.message){
	case WM_NCCREATE:
		return dispatch_message_to_(&target::on_pre_create_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->create.fire_(e);
			
			if (e.prevent_default)
				e.result = FALSE;
		});
	case WM_CREATE:
		return dispatch_message_to_(&target::on_create_, msg, target, [this](events::message &e, bool fire){
			if (!fire && e.prevent_default)
				e.result = -1;
		});
	case WM_DESTROY:
		return dispatch_message_to_(&target::on_destroy_, msg, target);
	case WM_NCDESTROY:
		return dispatch_message_to_(&target::on_post_destroy_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->destroy.fire_(e);
		});
	case WM_CLOSE:
		return dispatch_message_to_(&target::on_close_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->close.fire_(e);
		});
	case WM_MOUSEACTIVATE:
		return dispatch_message_to_(&target::on_mouse_activate_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_activate.fire_(e);

			if (e.prevent_default)//Prevent
				e.result = MA_NOACTIVATEANDEAT;
		});
	case WM_NCACTIVATE:
		return dispatch_message_to_(&target::on_pre_activate_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->pre_activate.fire_(e);
		});
	case WM_ACTIVATE:
		return dispatch_message_to_(&target::on_activate_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->activate.fire_(e);
		});
	case WM_CANCELMODE:
		return dispatch_message_to_(&target::on_cancel_mode_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->cancel_mode.fire_(e);
		});
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		return dispatch_message_to_(&target::on_focus_change_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->focus_change.fire_(e);
		});
	case WM_ENABLE:
		return dispatch_message_to_(&target::on_enable_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->enable.fire_(e);
		});
	case WM_SETCURSOR:
		return dispatch_message_to_(&target::on_set_cursor_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->set_cursor.fire_(e);
		});
	case EWIN_WM_GET_CURSOR:
		return dispatch_message_to_(&target::on_get_cursor_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->get_cursor.fire_(e);

			if (!e.handled)//Use default
				e.result = ::LoadCursorW(nullptr, IDC_ARROW);
		});
	case WM_NCHITTEST:
		return dispatch_message_to_(&target::on_hit_test_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->hit_test.fire_(e);

			if (e.prevent_default)//Prevent
				e.result = HTNOWHERE;
		});
	case WM_WINDOWPOSCHANGING:
	case WM_WINDOWPOSCHANGED:
	case EWIN_WM_POSITION_CHANGE:
		return dispatch_message_to_(&target::on_position_change_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->position_change.fire_(e);

			if (e.msg_->message == WM_WINDOWPOSCHANGING && e.prevent_default)//Prevent
				EWIN_SET(reinterpret_cast<common::types::wnd_position *>(e.msg_->lParam)->flags, (SWP_NOSIZE | SWP_NOMOVE));
		});
	case WM_SIZING:
	case WM_SIZE:
		return dispatch_message_to_(&target::on_size_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->size.fire_(e);

			if (e.msg_->message == WM_SIZING && e.prevent_default){//Prevent
				auto rect = reinterpret_cast<common::types::rect *>(e.msg_->lParam);
				common::types::size size = dynamic_cast<window::object *>(this)->size;
				*rect = common::types::rect{//Restore values
					(rect->left),
					(rect->top),
					(rect->left + size.cx),
					(rect->top + size.cy)
				};
			}
		});
	case WM_MOVING:
	case WM_MOVE:
		return dispatch_message_to_(&target::on_move_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->move.fire_(e);

			if (e.msg_->message == WM_MOVING && e.prevent_default)//Prevent
				*reinterpret_cast<common::types::rect *>(e.msg_->lParam) = dynamic_cast<window::object *>(this)->rect;
		});
	case WM_STYLECHANGING:
	case WM_STYLECHANGED:
		return dispatch_message_to_(&target::on_style_change_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->style_change.fire_(e);

			if (e.msg_->message == WM_STYLECHANGING && e.prevent_default){//Prevent
				auto info = reinterpret_cast<common::types::style_struct *>(e.msg_->lParam);
				info->styleNew = info->styleOld;
			}
		});
	case WM_ERASEBKGND:
		return dispatch_message_to_(&target::on_background_erase_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->background_erase.fire_(e);
		});
	case EWIN_WM_GET_BG_BRUSH:
		return dispatch_message_to_(&target::on_get_background_brush_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->get_background_brush.fire_(e);
		});
	case EWIN_WM_GET_BG_COLOR:
		return dispatch_message_to_(&target::on_get_background_color_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->get_background_color.fire_(e);
		});
	case WM_NCPAINT:
		return dispatch_message_to_(&target::on_non_client_paint_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->non_client_paint.fire_(e);
		});
	case WM_PAINT:
		return dispatch_message_to_(&target::on_paint_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->paint.fire_(e);
		});
	case WM_PRINTCLIENT:
		return dispatch_message_to_(&target::on_print_client_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->print_client.fire_(e);
		});
	case WM_NCMOUSELEAVE:
	case WM_MOUSELEAVE:
	case EWIN_WM_MOUSELEAVE:
		return dispatch_message_to_(&target::on_mouse_leave_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_leave.fire_(e);
		});
	case EWIN_WM_MOUSEENTER:
		return dispatch_message_to_(&target::on_mouse_enter_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_enter.fire_(e);
		});
	case WM_NCMOUSEMOVE:
		return dispatch_bubbling_message_to_(&target::on_mouse_move_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_move.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_MOUSEMOVE; });
	case WM_MOUSEMOVE:
		return dispatch_bubbling_message_to_(&target::on_mouse_move_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_move.fire_(e);
		});
	case WM_NCMOUSEHOVER:
		return dispatch_bubbling_message_to_(&target::on_mouse_hover_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_hover.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_MOUSEHOVER; });
	case WM_MOUSEHOVER:
		return dispatch_bubbling_message_to_(&target::on_mouse_hover_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_hover.fire_(e);
		});
	case WM_NCLBUTTONDOWN:
		return dispatch_bubbling_message_to_(&target::on_left_mouse_down_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->left_mouse_down.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_LBUTTONDOWN; });
	case WM_LBUTTONDOWN:
		return dispatch_bubbling_message_to_(&target::on_left_mouse_down_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->left_mouse_down.fire_(e);
		});
	case WM_NCMBUTTONDOWN:
		return dispatch_bubbling_message_to_(&target::on_middle_mouse_down_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->middle_mouse_down.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_MBUTTONDOWN; });
	case WM_MBUTTONDOWN:
		return dispatch_bubbling_message_to_(&target::on_middle_mouse_down_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->middle_mouse_down.fire_(e);
		});
	case WM_NCRBUTTONDOWN:
		return dispatch_bubbling_message_to_(&target::on_right_mouse_down_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->right_mouse_down.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_RBUTTONDOWN; });
	case WM_RBUTTONDOWN:
		return dispatch_bubbling_message_to_(&target::on_right_mouse_down_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->right_mouse_down.fire_(e);
		});
	case WM_NCLBUTTONUP:
		return dispatch_bubbling_message_to_(&target::on_left_mouse_up_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->left_mouse_up.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_LBUTTONUP; });
	case WM_LBUTTONUP:
		return dispatch_bubbling_message_to_(&target::on_left_mouse_up_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->left_mouse_up.fire_(e);
		});
	case WM_NCMBUTTONUP:
		return dispatch_bubbling_message_to_(&target::on_middle_mouse_up_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->middle_mouse_up.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_MBUTTONUP; });
	case WM_MBUTTONUP:
		return dispatch_bubbling_message_to_(&target::on_middle_mouse_up_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->middle_mouse_up.fire_(e);
		});
	case WM_NCRBUTTONUP:
		return dispatch_bubbling_message_to_(&target::on_right_mouse_up_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->right_mouse_up.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_RBUTTONUP; });
	case WM_RBUTTONUP:
		return dispatch_bubbling_message_to_(&target::on_right_mouse_up_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->right_mouse_up.fire_(e);
		});
	case WM_NCLBUTTONDBLCLK:
		return dispatch_bubbling_message_to_(&target::on_left_mouse_double_click_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->left_mouse_double_click.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_LBUTTONDBLCLK; });
	case WM_LBUTTONDBLCLK:
		return dispatch_bubbling_message_to_(&target::on_left_mouse_double_click_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->left_mouse_double_click.fire_(e);
		});
	case WM_NCMBUTTONDBLCLK:
		return dispatch_bubbling_message_to_(&target::on_middle_mouse_double_click_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->middle_mouse_double_click.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_MBUTTONDBLCLK; });
	case WM_MBUTTONDBLCLK:
		return dispatch_bubbling_message_to_(&target::on_middle_mouse_double_click_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->middle_mouse_double_click.fire_(e);
		});
	case WM_NCRBUTTONDBLCLK:
		return dispatch_bubbling_message_to_(&target::on_right_mouse_double_click_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->right_mouse_double_click.fire_(e);
		}, [](common::types::msg &msg){ msg.message = WM_RBUTTONDBLCLK; });
	case WM_RBUTTONDBLCLK:
		return dispatch_bubbling_message_to_(&target::on_right_mouse_double_click_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->right_mouse_double_click.fire_(e);
		});
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		return dispatch_bubbling_message_to_(&target::on_mouse_wheel_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_wheel.fire_(e);
		});
	case EWIN_WM_MOUSEDRAG:
		return dispatch_bubbling_message_to_(&target::on_mouse_drag_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_drag.fire_(e);
		});
	case EWIN_WM_MOUSEDRAGEND:
		return dispatch_bubbling_message_to_(&target::on_mouse_drag_end_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->mouse_drag_end.fire_(e);
		});
	case WM_CONTEXTMENU:
		return dispatch_bubbling_message_to_(&target::on_context_menu_, msg, target, [this](events::message &e, bool fire){
			if (!fire && !e.handled){
				e.result = 0;//Prevent procedure call

				auto context_menu_event = dynamic_cast<events::context_menu *>(&e);
				auto menu = context_menu_event->menu_;

				if (menu != nullptr && menu->tree.children.size > 0u){
					if (context_menu_event->position.x == -1 && context_menu_event->position.y == -1)
						menu->tracker.position = dynamic_cast<window::object *>(this)->position;
					else//Use specified position
						menu->tracker.position = context_menu_event->position;
					menu->tracker.activate = true;
				}
			}
			else if (fire && has_events_())
				get_events_()->context_menu.fire_(e);
		});
	case WM_KEYDOWN:
		return dispatch_bubbling_message_to_(&target::on_key_down_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->key_down.fire_(e);
		});
	case WM_KEYUP:
		return dispatch_bubbling_message_to_(&target::on_key_up_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->key_up.fire_(e);
		});
	case WM_CHAR:
		return dispatch_bubbling_message_to_(&target::on_key_press_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->key_press.fire_(e);
		});
	case WM_DEADCHAR:
		return dispatch_bubbling_message_to_(&target::on_dead_key_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->dead_key.fire_(e);
		});
	case WM_SYSKEYDOWN:
		return dispatch_bubbling_message_to_(&target::on_system_key_down_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->system_key_down.fire_(e);
		});
	case WM_SYSKEYUP:
		return dispatch_bubbling_message_to_(&target::on_system_key_up_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->system_key_up.fire_(e);
		});
	case WM_SYSCHAR:
		return dispatch_bubbling_message_to_(&target::on_system_key_press_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->system_key_press.fire_(e);
		});
	case WM_SYSDEADCHAR:
		return dispatch_bubbling_message_to_(&target::on_dead_system_key_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->dead_system_key.fire_(e);
		});
	case WM_SETTEXT:
		return dispatch_message_to_(&target::on_text_change_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->text_change.fire_(e);
		});
	case WM_SETFONT:
		return dispatch_message_to_(&target::on_font_change_, msg, target, [this](events::message &e, bool fire){
			if (fire && has_events_())
				get_events_()->font_change.fire_(e);
		});
	default:
		break;
	}

	return dispatch_message_to_(&target::on_unknown_message_, msg, target);
}

bool ewin::message::target::on_pre_create_(events::message &e){
	return true;
}

bool ewin::message::target::on_create_(events::message &e){
	return true;
}

void ewin::message::target::on_destroy_(events::message &e){}

void ewin::message::target::on_post_destroy_(events::message &e){}

bool ewin::message::target::on_close_(events::message &e){
	return true;
}

ewin::common::types::uint ewin::message::target::on_mouse_activate_(events::mouse_activate &e){
	return static_cast<common::types::uint>(call_procedure_(e));
}

bool ewin::message::target::on_pre_activate_(events::pre_activate &e){
	return true;
}

void ewin::message::target::on_activate_(events::activate &e){}

void ewin::message::target::on_cancel_mode_(events::message &e){}

void ewin::message::target::on_focus_change_(events::focus_change &e){}

void ewin::message::target::on_enable_(events::enable &e){}

void ewin::message::target::on_set_cursor_(events::cursor &e){
	auto result = ::SendMessageW(e.msg_->hwnd, EWIN_WM_GET_CURSOR, e.msg_->wParam, e.msg_->lParam);
	if (result != 0u)//Cursor returned
		::SetCursor(reinterpret_cast<common::types::hcursor>(result));
}

ewin::common::types::hcursor ewin::message::target::on_get_cursor_(events::cursor &e){
	switch (LOWORD(e.msg_->lParam)){
	case HTERROR://Play beep if applicable
		switch (HIWORD(e.msg_->lParam)){
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
			::MessageBeep(0);
			break;
		default:
			break;
		}

		return nullptr;
	case HTCLIENT://Use class cursor
		return reinterpret_cast<common::types::hcursor>(::GetClassLongPtrW(e.msg_->hwnd, GCLP_HCURSOR));
	case HTLEFT:
	case HTRIGHT:
		return ::LoadCursorW(nullptr, IDC_SIZEWE);
	case HTTOP:
	case HTBOTTOM:
		return ::LoadCursorW(nullptr, IDC_SIZENS);
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		return ::LoadCursorW(nullptr, IDC_SIZENWSE);
	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		return ::LoadCursorW(nullptr, IDC_SIZENESW);
	default:
		break;
	}

	return ::LoadCursorW(nullptr, IDC_ARROW);
}

ewin::common::types::uint ewin::message::target::on_hit_test_(events::hit_test &e){
	return static_cast<common::types::uint>(call_procedure_(e));
}

void ewin::message::target::on_position_change_(events::position_change &e){}

void ewin::message::target::on_size_(events::size &e){}

void ewin::message::target::on_move_(events::move &e){}

void ewin::message::target::on_style_change_(events::style_change &e){}

void ewin::message::target::on_background_erase_(events::draw &e){
	auto result = ::SendMessageW(e.msg_->hwnd, EWIN_WM_GET_BG_BRUSH, e.msg_->wParam, e.msg_->lParam);
	if (result == 0u){//Use color
		if ((result = ::SendMessageW(e.msg_->hwnd, EWIN_WM_GET_BG_COLOR, e.msg_->wParam, e.msg_->lParam)) == 0u)
			return;//Color not returned

		::SendMessageW(e.msg_->hwnd, EWIN_WM_POSITION_CHANGE, 0, 0);
		if (e.clip != dynamic_cast<window::object *>(this)->client_rect){
			drawing::solid_color_brush *brush = e.color_brush;
			brush->color = *reinterpret_cast<drawing::types::color *>(result);//Update

			common::types::rect clip = e.clip;
			e.drawer->native->FillRectangle(
				drawing::d2d1::RectF(
					((static_cast<float>(clip.left)		/ e.drawer->dpi.x) * 96.0f),
					((static_cast<float>(clip.top)		/ e.drawer->dpi.y) * 96.0f),
					((static_cast<float>(clip.right)	/ e.drawer->dpi.x) * 96.0f),
					((static_cast<float>(clip.bottom)	/ e.drawer->dpi.x) * 96.0f)
				),
				brush->native
			);
		}
		else//Clear entire area
			e.drawer->clear = *reinterpret_cast<drawing::types::color *>(result);
	}
	else{//Use brush
		::SendMessageW(e.msg_->hwnd, EWIN_WM_POSITION_CHANGE, 0, 0);
		common::types::rect clip = e.clip;
		e.drawer->native->FillRectangle(
			drawing::d2d1::RectF(
				((static_cast<float>(clip.left)		/ e.drawer->dpi.x) * 96.0f),
				((static_cast<float>(clip.top)		/ e.drawer->dpi.y) * 96.0f),
				((static_cast<float>(clip.right)	/ e.drawer->dpi.x) * 96.0f),
				((static_cast<float>(clip.bottom)	/ e.drawer->dpi.x) * 96.0f)
			),
			reinterpret_cast<drawing::brush *>(result)->native
		);
	}

	e.result = TRUE;//Handled
}

ewin::drawing::brush *ewin::message::target::on_get_background_brush_(events::message &e){
	return nullptr;
}

ewin::drawing::types::color *ewin::message::target::on_get_background_color_(events::message &e){
	return &background_color_;
}

void ewin::message::target::on_non_client_paint_(events::draw &e){}

void ewin::message::target::on_paint_(events::draw &e){}

void ewin::message::target::on_print_client_(events::draw &e){}

void ewin::message::target::on_mouse_leave_(events::mouse &e){}

void ewin::message::target::on_mouse_enter_(events::mouse &e){}

void ewin::message::target::on_mouse_move_(events::mouse &e){}

void ewin::message::target::on_mouse_hover_(events::mouse &e){}

void ewin::message::target::on_left_mouse_down_(events::mouse &e){}

void ewin::message::target::on_middle_mouse_down_(events::mouse &e){}

void ewin::message::target::on_right_mouse_down_(events::mouse &e){}

void ewin::message::target::on_left_mouse_up_(events::mouse &e){}

void ewin::message::target::on_middle_mouse_up_(events::mouse &e){}

void ewin::message::target::on_right_mouse_up_(events::mouse &e){}

void ewin::message::target::on_left_mouse_double_click_(events::mouse &e){}

void ewin::message::target::on_middle_mouse_double_click_(events::mouse &e){}

void ewin::message::target::on_right_mouse_double_click_(events::mouse &e){}

void ewin::message::target::on_mouse_wheel_(events::mouse &e){}

void ewin::message::target::on_mouse_drag_(events::mouse_drag &e){}

void ewin::message::target::on_mouse_drag_end_(events::mouse &e){}

void ewin::message::target::on_context_menu_(events::context_menu &e){}

void ewin::message::target::on_key_down_(events::key_down &e){}

void ewin::message::target::on_system_key_down_(events::key_down &e){}

void ewin::message::target::on_key_up_(events::key &e){}

void ewin::message::target::on_system_key_up_(events::key &e){}

void ewin::message::target::on_key_press_(events::key_press &e){}

void ewin::message::target::on_system_key_press_(events::key_press &e){}

void ewin::message::target::on_dead_key_(events::key_press &e){}

void ewin::message::target::on_dead_system_key_(events::key_press &e){}

void ewin::message::target::on_text_change_(events::message &e){}

void ewin::message::target::on_font_change_(events::message &e){}

void ewin::message::target::on_unknown_message_(events::message &e){}

ewin::common::types::result ewin::message::target::call_procedure_(events::message &e){
	return ::CallWindowProcW(procedure_, e.msg_->hwnd, e.msg_->message, e.msg_->wParam, e.msg_->lParam);
}
