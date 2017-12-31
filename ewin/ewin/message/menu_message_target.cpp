#include "../menu/menu_object.h"

ewin::message::menu_target::menu_target(){
	dispatch_message.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		auto info = static_cast<std::pair<common::types::msg *, common::types::result> *>(arg);
		info->second = dispatch_message_(*info->first, nullptr);
	});
}

ewin::message::menu_target::~menu_target() = default;

ewin::common::types::result ewin::message::menu_target::dispatch_message_(common::types::msg &msg, menu_target *target){
	switch (msg.message){
	case EWIN_WM_MENU_CREATE:
		return dispatch_message_to_(&menu_target::on_create_, msg, target, [this](events::menu_message &e, bool fire){
			if (fire && has_events_())
				get_events_()->create.fire_(e);
		});
	case EWIN_WM_MENU_DESTROY:
		return dispatch_message_to_(&menu_target::on_destroy_, msg, target, [this](events::menu_message &e, bool fire){
			if (fire && has_events_())
				get_events_()->destroy.fire_(e);
		});
	case EWIN_WM_MENU_INIT:
		return dispatch_bubbling_message_to_(&menu_target::on_init_, msg, target, [this](events::menu_message &e, bool fire){
			if (fire && has_events_())
				get_events_()->init.fire_(e);
			else if (!fire)//Accepted
				e.result = 1;
		});
	case EWIN_WM_MENU_HIGHLIGHT:
		return dispatch_bubbling_message_to_(&menu_target::on_highlight_, msg, target, [this](events::menu_message &e, bool fire){
			if (fire && has_events_())
				get_events_()->highlight.fire_(e);
		});
	case EWIN_WM_MENU_SELECT:
		return dispatch_bubbling_message_to_(&menu_target::on_select_, msg, target, [this](events::menu_message &e, bool fire){
			if (fire && has_events_())
				get_events_()->select.fire_(e);
		});
	case EWIN_WM_MENU_CHECK:
		return dispatch_bubbling_message_to_(&menu_target::on_check_, msg, target, [this](events::menu_message &e, bool fire){
			if (fire && has_events_())
				get_events_()->check.fire_(e);
		});
	default:
		break;
	}

	return dispatch_message_to_(&menu_target::on_unknown_message_, msg, target, [this](events::menu_message &e, bool fire){
		if (fire)
			get_events_()->unknown_message.fire_(e);
	});
}

void ewin::message::menu_target::on_create_(ewin::events::menu_message &e){}

void ewin::message::menu_target::on_destroy_(ewin::events::menu_message &e){}

void ewin::message::menu_target::on_highlight_(ewin::events::menu_message &e){}

void ewin::message::menu_target::on_select_(ewin::events::menu_message &e){}

void ewin::message::menu_target::on_check_(ewin::events::menu_message &e){}

bool ewin::message::menu_target::on_init_(ewin::events::menu_message &e){
	return true;
}

void ewin::message::menu_target::on_unknown_message_(ewin::events::menu_message &e){}
