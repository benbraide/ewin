#include "application_manager.h"

ewin::common::read_only_object_value_property<ewin::application::object, ewin::application::manager> ewin::application::manager::main;

ewin::common::read_only_object_value_property<ewin::application::object, ewin::application::manager> ewin::application::manager::current;

ewin::application::object *ewin::application::manager::main_ = nullptr;

thread_local std::shared_ptr<ewin::application::object> ewin::application::manager::current_;

void ewin::application::manager::bind_properties_(){
	main.initialize_(main_, nullptr);
	current.initialize_(nullptr, &handle_property_);
}

void ewin::application::manager::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &current){
		if (current_ == nullptr)
			current_.reset(new object);
		*reinterpret_cast<object **>(arg) = current_.get();
	}
}

ewin::application::manager_initializer::manager_initializer(){
	if (manager::main_ == nullptr){
		initialized_ = true;

		manager::current_.reset(new object);
		manager::main_ = manager::current_.get();

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

		::CoInitializeEx(nullptr, ::COINIT::COINIT_APARTMENTTHREADED);
	}
	else//Already initialized
		initialized_ = false;
}

ewin::application::manager_initializer::~manager_initializer(){
	if (initialized_)
		::CoUninitialize();
}

ewin::window::wnd_class ewin::application::manager_initializer::general_window_class;

ewin::window::wnd_class ewin::application::manager_initializer::dialog_window_class;

const ewin::application::manager_initializer ewin::application::manager_initializer::manager_initializer_;
