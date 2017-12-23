#include "../window/window_object.h"

ewin::events::object::object(target_type *target, target_type *owner)
	: target_((target == nullptr) ? owner : target), owner_(owner), states_(state_type::nil){
	bind_properties_();
	if (target_ != owner_)
		EWIN_SET(states_, state_type::bubbled);
}

void ewin::events::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	target.initialize_(target_, nullptr);
	owner.initialize_(owner_, nullptr);

	do_default.initialize_(nullptr, handler);
	prevent_default.initialize_(nullptr, handler);
	stop_propagation.initialize_(nullptr, handler);

	bubbled.initialize_(nullptr, handler);
	handled.initialize_(nullptr, handler);
}

void ewin::events::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &prevent_default){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::default_prevented);
		else if (access == common::property_access::write && *static_cast<bool *>(arg))
			prevent_default_();
	}
	else if (prop == &stop_propagation){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::propagation_stopped);
		else if (access == common::property_access::write && *static_cast<bool *>(arg))
			EWIN_SET(states_, state_type::propagation_stopped);
	}
	else if (prop == &bubbled)
		*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::bubbled);
	else if (prop == &handled)
		*static_cast<bool *>(arg) = EWIN_IS_ANY(states_, state_type::default_called | state_type::default_prevented | state_type::bubbled);
}

void ewin::events::object::do_default_(){
	if (!EWIN_IS_ANY(states_, state_type::default_called | state_type::default_prevented | state_type::bubbled))
		EWIN_SET(states_, state_type::default_called);
}

void ewin::events::object::prevent_default_(){
	if (!EWIN_IS_ANY(states_, state_type::default_called | state_type::default_prevented | state_type::bubbled))
		EWIN_SET(states_, state_type::default_prevented);
}

ewin::events::message::~message() = default;

void ewin::events::message::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &result){
		if (access == common::property_access::write){
			result_ = *static_cast<common::types::result *>(arg);
			EWIN_SET(states_, state_type::default_called);
		}
		else if (access == common::property_access::read)
			*static_cast<common::types::result *>(arg) = call_default_();
	}
	else//Forward
		object::handle_property_(prop, arg, access);
}

void ewin::events::message::do_default_(){
	call_default_();
}

ewin::common::types::result ewin::events::message::call_default_(){
	if (EWIN_IS_ANY(states_, state_type::default_called | state_type::default_prevented | state_type::bubbled))
		return result_;

	result_ = callback_(*this);//Call
	if (!EWIN_IS(states_, state_type::default_prevented))
		EWIN_SET(states_, state_type::default_called);

	return result_;
}

void ewin::events::message::bubble_(){
	EWIN_SET(states_, state_type::bubbled);
}

void ewin::events::message::remove_bubble_(){
	EWIN_REMOVE(states_, state_type::bubbled);
}

void ewin::events::mouse_activate::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &hit_target)
		*static_cast<common::types::uint *>(arg) = LOWORD(msg_->lParam);
	else if (prop == &mouse_message)
		*static_cast<common::types::uint *>(arg) = HIWORD(msg_->lParam);
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::pre_activate::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &is_activating)
		*static_cast<bool *>(arg) = EWIN_CPP_BOOL(msg_->wParam);
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::activate::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &is_activated)
		*static_cast<common::types::uint *>(arg) = (msg_->wParam != WA_INACTIVE);
	else if (prop == &is_click_activated)
		*static_cast<common::types::uint *>(arg) = (msg_->wParam == WA_CLICKACTIVE);
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::enable::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &is_enabled)
		*static_cast<bool *>(arg) = EWIN_CPP_BOOL(msg_->wParam);
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::hit_test::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &mouse_position)
		*static_cast<common::types::point *>(arg) = common::types::point{ GET_X_LPARAM(msg_->lParam), GET_Y_LPARAM(msg_->lParam) };
	else//Forward
		message::handle_property_(prop, arg, access);
}

ewin::events::style_change::~style_change(){
	auto info = reinterpret_cast<common::types::style_struct *>(msg_->lParam);
	if (info->styleNew != info->styleOld){//Filter styles
		auto window_target = dynamic_cast<window_type *>(target_);
		if (msg_->wParam == GWL_EXSTYLE){
			auto styles_removed = window_target->filter_extended_styles[EWIN_REMOVE_V(info->styleOld, info->styleNew)];
			auto styles_added = window_target->filter_extended_styles[EWIN_REMOVE_V(info->styleNew, info->styleOld)];
			info->styleNew = EWIN_REMOVE_V(EWIN_SET_V(info->styleOld, styles_added), styles_removed);
		}
		else{//Normal styles
			auto styles_removed = window_target->filter_styles[EWIN_REMOVE_V(info->styleOld, info->styleNew)];
			auto styles_added = window_target->filter_styles[EWIN_REMOVE_V(info->styleNew, info->styleOld)];
			info->styleNew = EWIN_REMOVE_V(EWIN_SET_V(info->styleOld, styles_added), styles_removed);
		}
	}
}

void ewin::events::style_change::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &is_extended)
		*static_cast<common::types::uint *>(arg) = (msg_->wParam == GWL_EXSTYLE);
	else if (prop == &styles)
		*static_cast<common::types::style_struct **>(arg) = reinterpret_cast<common::types::style_struct *>(msg_->lParam);
	else//Forward
		base_type::handle_property_(prop, arg, access);
}

ewin::events::draw::~draw(){
	if (cleanup_ != nullptr)
		cleanup_();

	if (drawer_ != nullptr)
		drawer_->began = false;
}

void ewin::events::draw::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &drawer){
		begin_paint_();
		*static_cast<drawing::object **>(arg) = drawer_;
	}
	else if (prop == &color_brush){
		begin_paint_();
		*static_cast<drawing::solid_color_brush **>(arg) = color_brush_;
	}
	else if (prop == &clip){
		begin_paint_();
		*static_cast<common::types::rect *>(arg) = info_.rcPaint;
	}
	else if (prop == &erase_background){
		begin_paint_();
		*static_cast<bool *>(arg) = EWIN_CPP_BOOL(info_.fErase);
	}
	else if (prop == &paint_info){
		begin_paint_();
		*static_cast<common::types::paint_struct **>(arg) = &info_;
	}
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::draw::begin_paint_(){
	if (drawer_ != nullptr)
		return;//Not applicable

	switch (msg_->message){
	case WM_NCPAINT:
	{
		auto drawer = static_cast<drawing::hdc_object *>(dynamic_cast<window::object *>(target_)->app->hdc_drawer);
		::GetClipBox(info_.hdc = GetDCEx(msg_->hwnd, reinterpret_cast<common::types::hrgn>(msg_->wParam), DCX_WINDOW | DCX_INTERSECTRGN), &info_.rcPaint);
		bind_(*drawer);
		drawer_ = drawer;
		color_brush_ = dynamic_cast<window::object *>(target_)->color_brush;
		cleanup_ = [this]{
			::ReleaseDC(msg_->hwnd, info_.hdc);
		};
		break;
	}
	case WM_PAINT:
	{
		::BeginPaint(msg_->hwnd, &info_);
		drawer_ = static_cast<drawing::hwnd_object *>(dynamic_cast<window::object *>(target_)->drawer);
		color_brush_ = dynamic_cast<window::object *>(target_)->color_brush;
		cleanup_ = [this]{
			::EndPaint(msg_->hwnd, &info_);
		};
		break;
	}
	case WM_PRINTCLIENT:
	{
		auto drawer = static_cast<drawing::hdc_object *>(dynamic_cast<window::object *>(target_)->app->hdc_drawer);
		::GetClipBox(info_.hdc = reinterpret_cast<common::types::hdc>(msg_->wParam), &info_.rcPaint);
		bind_(*drawer);
		drawer_ = drawer;
		color_brush_ = dynamic_cast<window::object *>(target_)->color_brush;
		info_.fErase = EWIN_IS(msg_->lParam, PRF_ERASEBKGND);
		break;
	}
	case WM_ERASEBKGND:
		::GetClipBox(info_.hdc = reinterpret_cast<common::types::hdc>(msg_->wParam), &info_.rcPaint);
		drawer_ = static_cast<drawing::hwnd_object *>(dynamic_cast<window::object *>(target_)->drawer);
		color_brush_ = dynamic_cast<window::object *>(target_)->color_brush;
		info_.fErase = TRUE;
		break;
	default:
		drawer_ = static_cast<drawing::hwnd_object *>(dynamic_cast<window::object *>(target_)->drawer);
		color_brush_ = dynamic_cast<window::object *>(target_)->color_brush;
		info_.rcPaint = dynamic_cast<window::object *>(target_)->client_rect;
		break;
	}

	drawer_->began = true;
	drawer_->transform = drawing::d2d1::IdentityMatrix();
}

void ewin::events::draw::bind_(drawing::hdc_object &drawer){
	drawer.target = nullptr;
	drawer.offset.x = info_.rcPaint.left;
	drawer.offset.y = info_.rcPaint.top;
	drawer.size.width = (info_.rcPaint.right - info_.rcPaint.left);
	drawer.size.height = (info_.rcPaint.bottom - info_.rcPaint.top);
	drawer.target = info_.hdc;
}

ewin::events::mouse::~mouse(){
	if (!EWIN_IS(states_, state_type::bubbled))
		application::manager::current->update_last_mouse_position = true;
}

void ewin::events::mouse::cache_values_(){
	auto position = ::GetMessagePos();
	cache_.position = common::types::point{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };

	switch (msg_->message){
	case WM_NCMOUSEMOVE:
	case WM_NCMOUSEHOVER:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_NCMBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCXBUTTONDOWN:
	case WM_NCXBUTTONUP:
	case WM_NCXBUTTONDBLCLK:
		cache_.hit = static_cast<common::types::uint>(msg_->wParam);
		cache_.delta = common::types::size{
			(cache_.position.x - application::manager::current->last_mouse_position.x),
			(cache_.position.y - application::manager::current->last_mouse_position.y)
		};
		break;
	case WM_NCMOUSELEAVE:
	case EWIN_WM_MOUSEENTER:
		cache_.hit = HTTOP;
		cache_.delta = common::types::size{};
		break;
	case WM_MOUSEWHEEL:
		cache_.hit = HTCLIENT;
		cache_.delta = common::types::size{ 0, static_cast<int>(HIWORD(msg_->wParam) / WHEEL_DELTA) };
		break;
	case WM_MOUSEHWHEEL:
		cache_.hit = HTCLIENT;
		cache_.delta = common::types::size{ static_cast<int>(HIWORD(msg_->wParam) / WHEEL_DELTA), 0 };
		break;
	default://Client area
		cache_.hit = HTCLIENT;
		cache_.delta = common::types::size{
			(cache_.position.x - application::manager::current->last_mouse_position.x),
			(cache_.position.y - application::manager::current->last_mouse_position.y)
		};
		break;
	}
}

ewin::events::mouse_drag::~mouse_drag() = default;
