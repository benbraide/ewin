#include "writing_localized_strings.h"

std::wstring ewin::writing::localized_strings::operator[](common::types::uint index) const{
	common::types::uint size;
	if (!SUCCEEDED(native_->GetStringLength(index, &size)) || size == 0u)
		return L"";//Error

	std::wstring value;
	value.resize(size);

	return (SUCCEEDED(native_->GetString(index, value.data(), size)) ? value : L"");
}

std::wstring ewin::writing::localized_strings::operator[](const wchar_t *locale) const{
	common::types::uint index;
	common::types::boolean exists;
	if (SUCCEEDED(native_->FindLocaleName(locale, &index, &exists)) && EWIN_CPP_BOOL(exists))
		return operator[](index);//Use index
	return L"";
}

std::wstring ewin::writing::localized_strings::operator[](const std::wstring &locale) const{
	return operator[](locale.data());
}

void ewin::writing::localized_strings::bind_properties_(){
	count.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		*static_cast<common::types::uint *>(arg) = native_->GetCount();
	});
}
