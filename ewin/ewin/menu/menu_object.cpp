#include "menu_object.h"

ewin::menu::object::object()
	: menu_target(events_), tree(*this), app_(nullptr), error_throw_policy_(error_throw_policy_type::always),
	error_value_(error_type::nil), local_error_value_(ERROR_SUCCESS), auto_destroy_(true), events_(*this){
	cache_ = cache_info{};
	bind_properties_();
}

ewin::menu::object::~object(){
	destruct_();
}

ewin::message::menu_target *ewin::menu::object::parent_() const{
	return tree.parent_;
}

void ewin::menu::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	reflect.initialize_(nullptr, handler);
	is_forbidden.initialize_(nullptr, handler);

	error_throw_policy.initialize_(nullptr, handler);
	error.initialize_(nullptr, handler);

	app.initialize_(nullptr, handler);
	handle.initialize_(nullptr, handler);

	size.initialize_(&cache_.size, handler);
	position.initialize_(&cache_.position, handler);
	rect.initialize_(&cache_.rect, handler);

	created.initialize_(nullptr, handler);
	create.initialize_(nullptr, handler);
	auto_destroy.initialize_(&auto_destroy_, nullptr);
}

void ewin::menu::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &reflect){
		*static_cast<ptr_type *>(arg) = reflect_();
		return;
	}

	if (prop == &is_forbidden){
		auto info = static_cast<std::pair<property_forbidden_info *, bool> *>(arg);
		info->second = is_forbidden_(*info->first);
		return;
	}

	if (prop == &error){
		if (access == common::property_access::read){
			auto &info = *static_cast<common::variant_value_property_arg_info *>(arg);
			switch (info.index){
			case 0u:
				*static_cast<error_type *>(info.value) = error_value_;
				break;
			case 1u:
				*static_cast<common::types::dword *>(info.value) = local_error_value_;
				break;
			default:
				break;
			}
		}
		else if (access == common::property_access::write)
			set_error_(*static_cast<common::variant_value_property_arg_info *>(arg));

		return;
	}

	if (is_forbidden_(property_forbidden_info{ prop, access })){
		set_error_(error_type::forbidden_property);
		return;
	}

	error_value_ = error_type::nil;
	if (prop == &error_throw_policy){
		if (access == common::property_access::read)
			*static_cast<error_throw_policy_type *>(arg) = error_throw_policy_;
		else if (access == common::property_access::write)
			error_throw_policy_ = *static_cast<error_throw_policy_type *>(arg);
	}
	else if (prop == &app){
		if (access == common::property_access::read)
			*static_cast<application_type **>(arg) = app_;
		else if (access == common::property_access::write && handle_value_() == nullptr)
			app_ = static_cast<application_type *>(arg);
	}
	else if (prop == &size && access == common::property_access::write){}
	else if (prop == &position && access == common::property_access::write){}
	else if (prop == &rect && access == common::property_access::write){}
	else if (prop == &created){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = (handle_value_() != nullptr);
		else if (access == common::property_access::write)
			create_(*static_cast<bool *>(arg), nullptr);
	}
	else if (prop == &create)
		create_(true, static_cast<create_info *>(arg));
}

ewin::menu::object::ptr_type ewin::menu::object::reflect_(){
	return shared_from_this();
}

ewin::common::types::hmenu ewin::menu::object::handle_value_(){
	return nullptr;
}

bool ewin::menu::object::is_forbidden_(const property_forbidden_info &info){
	if (info.value == &app_ && info.access == common::property_access::write)
		return (handle_value_() != nullptr);//Prevent app change after creation
	return false;
}

void ewin::menu::object::destruct_(){
	if (auto_destroy_){
		try{
			create_(false, nullptr);
		}
		catch (...){}
	}

	if (!tree.children_.empty()){
		auto children = tree.children_;//Get copy of list
		for (auto child : children)//Remove association
			child->tree.set_parent_(nullptr, 0);
	}

	tree.set_parent_(nullptr, 0);//Remove parent, if any
}

void ewin::menu::object::create_(bool create, const create_info *info){}

void ewin::menu::object::set_error_(common::variant_value_property_arg_info &info){
	switch (info.index){
	case 0u:
		set_error_(*static_cast<error_type *>(info.value));
		break;
	case 1u:
		set_error_(*static_cast<common::types::dword *>(info.value));
		break;
	default:
		break;
	}
}

void ewin::menu::object::set_error_(error_type value){
	if (value == error_type::nil){//Clear error
		error_value_ = value;
		return;
	}

	switch (static_cast<error_throw_policy_type>(error_throw_policy_)){
	case error_throw_policy_type::never://Update last error
		error_value_ = value;
		break;
	case error_throw_policy_type::once://Throw once
		error_throw_policy_ = error_throw_policy_type::never;
		throw value;
		break;
	default:
		throw value;
		break;
	}
}

void ewin::menu::object::set_error_(common::types::dword value){
	if ((local_error_value_ = value) == ERROR_SUCCESS)
		error_value_ = error_type::nil;
	else//Signify local error
		set_error_(error_type::local_error);
}

bool ewin::menu::object::validate_parent_change_(object *value){
	return true;
}

bool ewin::menu::object::validate_child_remove_(object &value){
	return true;
}

bool ewin::menu::object::validate_child_add_(object &value, std::size_t index){
	return true;
}

void ewin::menu::object::child_removed_(object &value, std::size_t index){}

void ewin::menu::object::child_added_(object &value, std::size_t index){}

void ewin::menu::object::parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index){}

void ewin::menu::object::event_listener_count_changed_(events::menu_basic &e, std::size_t count){}
