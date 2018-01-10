#include "control_object.h"

ewin::window::control::object::object()
	: font(std::bind(&object::font_changed_, this, std::placeholders::_1)), format_(nullptr), layout_(nullptr), rss_app_(nullptr){
	view.visible = true;
	font.handle = static_cast<writing::types::gdi_font>(::GetStockObject(SYSTEM_FONT));
	cache_.size = common::types::size{ 10, 10 };
	relative_size_ = drawing::types::size{ 1.0f, 1.0f };
	padding_ = common::types::size{ 10, 4 };
	bind_properties_();
}

ewin::window::control::object::~object(){
	destruct_();
	free_rss_();
}

void ewin::window::control::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	id.initialize_(&id_, nullptr);
	label.initialize_(&label_, handler);

	relative_size.initialize_(&relative_size_, handler);
	padding.initialize_(&padding_, handler);
}

void ewin::window::control::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop != &relative_size && prop != &padding){
		if (prop == &label){
			::SendMessageW(handle_, WM_SETTEXT, 0, reinterpret_cast<common::types::wparam>(label_.data()));
		}
		else if (prop == &size || prop == &client_size || prop == &rect || prop == &relative_rect || prop == &client_rect){
			relative_size_.width = relative_size_.height = 0.0f;//Clear relative size
			base_type::handle_property_(prop, arg, access);
		}
		else
			base_type::handle_property_(prop, arg, access);
	}
	else//Resize if size is relative
		resize_();
}

void ewin::window::control::object::create_(bool create, const typename create_info *info){
	if (!create){//Forward message
		base_type::create_(create, info);
		return;
	}

	common::types::size size{};
	common::types::point position{};

	auto options = attribute_option_type::nil;
	if (cache_.position.x != 0 || cache_.position.y != 0){
		options = attribute_option_type::absolute_position;
		position = cache_.position;
	}
	else//Relative position
		position = cache_.relative_position;

	if (cache_.size.cx == 0 && cache_.size.cy == 0){
		if (cache_.client_size.cx != 0 || cache_.client_size.cy != 0)
			EWIN_SET(options, attribute_option_type::client_size);
		size = cache_.client_size;
	}
	else//Full size
		size = cache_.size;

	base_type::low_level_create_(common::types::create_struct{
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		size.cx,
		size.cy,
		position.y,
		position.x,
		0,
		label_.data(),
		nullptr,
		0u
	}, nullptr, nullptr, options);

	if (handle_ != nullptr)//Set font
		::SendMessageW(handle_, WM_SETFONT, reinterpret_cast<common::types::wparam>(static_cast<writing::types::gdi_font>(font.handle)), TRUE);
}

void ewin::window::control::object::on_text_change_(events::message &e){
	if (e.info->lParam != reinterpret_cast<common::types::wparam>(label_.data()))
		label_ = reinterpret_cast<const wchar_t *>(e.info->lParam);
	resize_();
}

void ewin::window::control::object::on_font_change_(events::message &e){
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
	if (layout == nullptr && handle_ != nullptr){//Use GDI
		app_->task += [this]{
			auto wdc = ::GetDC(handle_);
			auto old = ::SelectObject(wdc, font.handle);

			common::types::size size, alphabets_size;
			if (EWIN_CPP_BOOL(::GetTextExtentPoint32W(wdc, label_.c_str(), static_cast<int>(label_.size()), &size))){
				if (EWIN_CPP_BOOL(::GetTextExtentPoint32W(wdc, alphabets, (26 * 2), &alphabets_size))){
					cache_.size = common::types::size{
						static_cast<int>((relative_size_.width * size.cx) + padding_.cx),
						static_cast<int>((relative_size_.height * alphabets_size.cy) + padding_.cy)
					};
				}
			}

			::SelectObject(wdc, old);
			::ReleaseDC(handle_, wdc);
		};
	}
	else if (layout != nullptr){
		writing::types::text_metrics metrics{};
		if (!SUCCEEDED(layout->GetMetrics(&metrics)))
			return;//Failed to retrieve metrics

		drawing::types::point dpi{};
		app_->drawing_factory->native->GetDesktopDpi(&dpi.x, &dpi.y);

		drawing::types::point dpi_scale{
			(dpi.x / 96.0f),
			(dpi.y / 96.0f)
		};

		cache_.size = common::types::size{
			static_cast<int>((relative_size_.width * (metrics.width * dpi_scale.x)) + padding_.cx),
			static_cast<int>((relative_size_.height * (metrics.height * dpi_scale.y)) + padding_.cy)
		};
	}

	compute_additional_size_();
}

void ewin::window::control::object::compute_additional_size_(){}

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

const wchar_t *ewin::window::control::object::alphabets = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
