#include "../window/message_window.h"

#include "main_application.h"

ewin::application::main_app::main_app()
	: object(true){
	manager::main_ = this;
	manager::bind_properties_();
	
	common::types::wnd_class info{ sizeof(common::types::wnd_class) };
	::GetClassInfoExW(nullptr, L"#32770", &info);//Retrieve dialog class info

	dialog_window_class.instance = ::GetModuleHandleW(nullptr);
	dialog_window_class.procedure = object::entry_;
	dialog_window_class.style = (info.style | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
	dialog_window_class.name = (L"EWIN_DLG_CLS_" EWIN_WUUID);
	dialog_window_class.background_brush = info.hbrBackground;
	dialog_window_class.cursor = info.hCursor;
	dialog_window_class.small_icon = info.hIconSm;
	dialog_window_class.icon = info.hIcon;
	dialog_window_class.wnd_extra = info.cbWndExtra;
	dialog_window_class.cls_extra = info.cbClsExtra;
	dialog_window_class.created = true;

	general_window_class.instance = dialog_window_class.instance;
	general_window_class.procedure = dialog_window_class.procedure;
	general_window_class.style = dialog_window_class.style;
	general_window_class.name = (L"EWIN_CLS_" EWIN_WUUID);
	general_window_class.cursor = dialog_window_class.cursor;
	general_window_class.created = true;

	message_window_->created = true;
	::CoInitializeEx(nullptr, ::COINIT::COINIT_APARTMENTTHREADED);
}

ewin::application::main_app::~main_app(){
	::CoUninitialize();
}
