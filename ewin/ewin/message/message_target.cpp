#include "../window/window_object.h"

ewin::message::target::target()
	: procedure_(::DefWindowProcW){
	dispatch_message.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		if (prop == &dispatch_message){
			auto info = static_cast<std::pair<common::types::msg *, common::types::result> *>(arg);
			info->second = dispatch_message_(*info->first);
		}
	});
}

ewin::message::target::~target() = default;

ewin::common::types::result ewin::message::target::dispatch_message_(common::types::msg &msg){
	switch (msg.message){
	case WM_NCCREATE:
		return dispatch_message_to_(&target::on_pre_create_, msg);
	case WM_CREATE:
		return dispatch_message_to_boolean_(&target::on_create_, msg, result_pair_type{ -1, 0 });
	case WM_DESTROY:
		return dispatch_message_to_(&target::on_destroy_, msg);
	case WM_NCDESTROY:
		return dispatch_message_to_(&target::on_post_destroy_, msg);
	case WM_CLOSE:
		if (EWIN_CPP_BOOL(dispatch_message_to_boolean_(&target::on_close_, msg)))
			dynamic_cast<window::object *>(this)->created = false;//Destroy window
		return 0;
	case WM_MOUSEACTIVATE:
		return dispatch_message_to_(&target::on_mouse_activate_, msg);
	case WM_NCACTIVATE:
		return dispatch_message_to_(&target::on_pre_activate_, msg);
	case WM_ACTIVATE:
		return dispatch_message_to_(&target::on_activate_, msg);
	case WM_CANCELMODE:
		return dispatch_message_to_(&target::on_cancel_mode_, msg);
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		return dispatch_message_to_(&target::on_focus_change_, msg);
	case WM_ENABLE:
		return dispatch_message_to_(&target::on_enable_, msg);
	case WM_SETCURSOR:
		return dispatch_message_to_(&target::on_set_cursor_, msg);
	case WM_NCHITTEST:
		return dispatch_message_to_(&target::on_hit_test_, msg);
	case WM_WINDOWPOSCHANGING:
	case WM_WINDOWPOSCHANGED:
		return dispatch_message_to_(&target::on_position_change_, msg);
	case WM_SIZING:
	case WM_SIZE:
		return dispatch_message_to_(&target::on_size_, msg);
	case WM_MOVING:
	case WM_MOVE:
		return dispatch_message_to_(&target::on_move_, msg);
	case WM_STYLECHANGING:
	case WM_STYLECHANGED:
		return dispatch_message_to_(&target::on_style_change_, msg);
	default:
		break;
	}

	return dispatch_message_to_(&target::on_unknown_message_, msg);
}

bool ewin::message::target::on_pre_create_(events::message &e){
	return EWIN_CPP_BOOL(::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam));
}

bool ewin::message::target::on_create_(events::message &e){
	return (::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam) == 0u);
}

void ewin::message::target::on_destroy_(events::message &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

void ewin::message::target::on_post_destroy_(events::message &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

bool ewin::message::target::on_close_(events::message &e){
	return true;
}

ewin::common::types::uint ewin::message::target::on_mouse_activate_(events::mouse_activate &e){
	return static_cast<common::types::uint>(::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam));
}

bool ewin::message::target::on_pre_activate_(events::pre_activate &e){
	return EWIN_CPP_BOOL(::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam));
}

void ewin::message::target::on_activate_(events::activate &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

void ewin::message::target::on_cancel_mode_(events::message &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

void ewin::message::target::on_focus_change_(events::focus_change &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

void ewin::message::target::on_enable_(events::enable &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

ewin::common::types::hcursor ewin::message::target::on_set_cursor_(events::set_cursor &e){
	return reinterpret_cast<common::types::hcursor>(::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam));
}

ewin::common::types::uint ewin::message::target::on_hit_test_(events::hit_test &e){
	return static_cast<common::types::uint>(::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam));
}

void ewin::message::target::on_position_change_(events::position_change &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

void ewin::message::target::on_size_(events::size &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

void ewin::message::target::on_move_(events::move &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

void ewin::message::target::on_style_change_(events::move &e){
	::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}

void ewin::message::target::on_unknown_message_(events::message &e){
	e.result = ::CallWindowProcW(procedure_, e.info->hwnd, e.info->message, e.info->wParam, e.info->lParam);
}
