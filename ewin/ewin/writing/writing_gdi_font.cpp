#include "writing_gdi_font.h"

ewin::writing::gdi_font::gdi_font()
	: handle_(nullptr), auto_destroy_(true){
	info_ = types::gdi_font_info{};
	bind_properties_();
}

ewin::writing::gdi_font::gdi_font(callback_type callback)
	: callback_(callback), handle_(nullptr), auto_destroy_(true){
	info_ = types::gdi_font_info{};
	bind_properties_();
}

ewin::writing::gdi_font::~gdi_font(){
	destroy_();
}

void ewin::writing::gdi_font::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(gdi_font);

	handle.initialize_(nullptr, handler);

	info.initialize_(nullptr, handler);
	face_name.initialize_(nullptr, handler);

	height.initialize_(nullptr, handler);
	weight.initialize_(nullptr, handler);

	escapement.initialize_(nullptr, handler);
	orientation.initialize_(nullptr, handler);

	italic.initialize_(nullptr, handler);
	underline.initialize_(nullptr, handler);
	strike_out.initialize_(nullptr, handler);

	created.initialize_(nullptr, handler);
	auto_destroy.initialize_(&auto_destroy_, nullptr);
}

void ewin::writing::gdi_font::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &handle){
		if (access == common::property_access::write && *static_cast<types::gdi_font *>(arg) != handle_){
			destroy_();
			if ((handle_ = *static_cast<types::gdi_font *>(arg)) != nullptr){//Retrieve info
				::GetObjectW(handle_, static_cast<int>(sizeof(types::gdi_font_info)), &info_);
				face_name_.clear();
				for (auto i = 0; info_.lfFaceName[i] != L'\0'; ++i)
					face_name_.append(1, info_.lfFaceName[i]);
			}

			auto_destroy_ = false;
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<types::gdi_font *>(arg) = get_handle_();
	}
	else if (prop == &info){
		if (access == common::property_access::write){
			info_ = *static_cast<types::gdi_font_info *>(arg);
			destroy_();

			face_name_.clear();
			for (auto i = 0; info_.lfFaceName[i] != L'\0'; ++i)
				face_name_.append(1, info_.lfFaceName[i]);

			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read){
			std::size_t i = 0;
			for (; i < 31u; ++i)//Copy face name
				info_.lfFaceName[i] = face_name_[i];
			info_.lfFaceName[i] = L'\0';
			*static_cast<types::gdi_font_info *>(arg) = info_;
		}
	}
	else if (prop == &face_name){
		if (access == common::property_access::write && *static_cast<std::wstring *>(arg) != face_name_){
			face_name_ = *static_cast<std::wstring *>(arg);
			destroy_();
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<std::wstring *>(arg) = face_name_;
	}
	else if (prop == &height){
		if (access == common::property_access::write && *static_cast<int *>(arg) != info_.lfHeight){
			info_.lfHeight = *static_cast<int *>(arg);
			destroy_();
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<int *>(arg) = info_.lfHeight;
	}
	else if (prop == &weight){
		if (access == common::property_access::write && *static_cast<int *>(arg) != info_.lfWeight){
			info_.lfWeight = *static_cast<int *>(arg);
			destroy_();
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<int *>(arg) = info_.lfWeight;
	}
	else if (prop == &escapement){
		if (access == common::property_access::write && *static_cast<int *>(arg) != info_.lfEscapement){
			info_.lfEscapement = *static_cast<int *>(arg);
			destroy_();
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<int *>(arg) = info_.lfEscapement;
	}
	else if (prop == &orientation){
		if (access == common::property_access::write && *static_cast<int *>(arg) != info_.lfOrientation){
			info_.lfOrientation = *static_cast<int *>(arg);
			destroy_();
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<int *>(arg) = info_.lfOrientation;
	}
	else if (prop == &italic){
		if (access == common::property_access::write && *static_cast<bool *>(arg) != EWIN_CPP_BOOL(info_.lfItalic)){
			info_.lfItalic = EWIN_C_BOOL(*static_cast<bool *>(arg));
			destroy_();
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<bool *>(arg) = EWIN_CPP_BOOL(info_.lfItalic);
	}
	else if (prop == &underline){
		if (access == common::property_access::write && *static_cast<bool *>(arg) != EWIN_CPP_BOOL(info_.lfUnderline)){
			info_.lfUnderline = EWIN_C_BOOL(*static_cast<bool *>(arg));
			destroy_();
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<bool *>(arg) = EWIN_CPP_BOOL(info_.lfUnderline);
	}
	else if (prop == &strike_out){
		if (access == common::property_access::write && *static_cast<bool *>(arg) != EWIN_CPP_BOOL(info_.lfStrikeOut)){
			info_.lfStrikeOut = EWIN_C_BOOL(*static_cast<bool *>(arg));
			destroy_();
			if (callback_ != nullptr)
				callback_(*this);
		}
		else if (access == common::property_access::read)
			*static_cast<bool *>(arg) = EWIN_CPP_BOOL(info_.lfStrikeOut);
	}
	else if (prop == &created){
		if (access == common::property_access::write){
			if (*static_cast<bool *>(arg))//Create
				get_handle_();
			else//Destroy
				destroy_();
		}
		else if (access == common::property_access::read)
			*static_cast<bool *>(arg) = (handle_ != nullptr);
	}
}

void ewin::writing::gdi_font::destroy_(){
	if (handle_ != nullptr){
		if (auto_destroy_)
			::DeleteObject(handle_);
		handle_ = nullptr;
	}
}

ewin::writing::types::gdi_font ewin::writing::gdi_font::get_handle_(){
	if (handle_ == nullptr){//Create
		handle_ = ::CreateFontW(
			info_.lfHeight,
			info_.lfWidth,
			info_.lfEscapement,
			info_.lfOrientation,
			info_.lfWeight,
			info_.lfItalic,
			info_.lfUnderline,
			info_.lfStrikeOut,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			face_name_.data()
		);

		if (handle_ != nullptr){
			auto_destroy_ = true;
			set_error_(common::error_type::nil);
		}
		else//Error
			set_error_(::GetLastError());
	}

	return handle_;
}
