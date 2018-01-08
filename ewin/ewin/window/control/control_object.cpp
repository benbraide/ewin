#include "control_object.h"

ewin::window::control::object::object()
	: font(std::bind(&object::font_changed_, this, std::placeholders::_1)), format_(nullptr), layout_(nullptr), rss_app_(nullptr){
	font.handle = static_cast<writing::types::gdi_font>(::GetStockObject(SYSTEM_FONT));
	bind_properties_();
}

ewin::window::control::object::~object(){
	destruct_();
	free_rss_();
}

void ewin::window::control::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	id.initialize_(&id_, nullptr);
	relative_size.initialize_(&relative_size_, handler);
	padding.initialize_(&padding_, handler);
}

void ewin::window::control::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop != &relative_size && prop != &padding){
		if (prop == &size || prop == &client_size || prop == &rect || prop == &relative_rect || prop == &client_rect)
			relative_size_.width = relative_size_.height = 0.0f;//Clear relative size
		base_type::handle_property_(prop, arg, access);
	}
	else//Resize if size is relative
		resize_();
}

void ewin::window::control::object::resize_(){
	if (handle_ != nullptr && (relative_size_.width > 0.0f || relative_size_.height > 0.0f)){//Update size
		compute_size_();
		::SetWindowPos(handle_, nullptr, 0, 0, cache_.size.cx, cache_.size.cy, (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE));
	}
}

void ewin::window::control::object::compute_size_(){
	auto layout = get_layout_();
	if (layout == nullptr){//Use GDI

	}
	else{//Use DWrite
		writing::types::text_metrics metrics{};
		layout->GetMetrics(&metrics);
		cache_.size = common::types::size{
			static_cast<int>(relative_size_.width * metrics.width + padding_.cx),
			static_cast<int>(relative_size_.height * metrics.height + padding_.cy)
		};
	}
}

ewin::writing::types::text_format *ewin::window::control::object::get_format_(){
	if (format_ == nullptr && handle_ != nullptr){//Create
		app_->writing_factory->native->CreateTextFormat(
			font.face_name_.data(),
			app_->font_collection,
			static_cast<writing::types::font_weight>(font.info_.lfWeight),
			writing::types::font_style::DWRITE_FONT_STYLE_NORMAL,
			writing::types::font_stretch::DWRITE_FONT_STRETCH_NORMAL,
			static_cast<float>(font.info_.lfHeight),
			L"en-us",
			&format_
		);
	}

	return format_;
}

ewin::writing::types::text_layout *ewin::window::control::object::get_layout_(){
	if (layout_ == nullptr && handle_ != nullptr && !label_.empty()){//Create
		app_->writing_factory->native->CreateTextLayout(
			label_.data(),
			static_cast<common::types::uint>(label_.size()),
			get_format_(),
			0,
			0,
			&layout_
		);
	}

	return layout_;
}

void ewin::window::control::object::free_rss_(){
	if (rss_app_ == nullptr)
		return;

	rss_app_->task += [this]{
		if (layout_ != nullptr){
			layout_->Release();
			layout_ = nullptr;
		}

		if (format_ != nullptr){
			format_->Release();
			format_ = nullptr;
		}

		rss_app_ = nullptr;
	};
}

void ewin::window::control::object::font_changed_(writing::gdi_font &font){
	free_rss_();
	if (handle_ != nullptr)//Update
		::SendMessageW(handle_, WM_SETFONT, reinterpret_cast<common::types::wparam>(static_cast<writing::types::gdi_font>(font.handle)), TRUE);
}
