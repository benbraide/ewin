#include "menu_object.h"

ewin::menu::object::object()
	: tree(*this), app_(nullptr), auto_destroy_(true){
	cache_ = cache_info{};
	bind_properties_();
}

ewin::menu::object::~object(){
	destruct_();
}

ewin::message::menu_target *ewin::menu::object::parent_() const{
	return tree.parent_;
}

ewin::menu::menu_event *ewin::menu::object::get_events_(){
	if (events_ == nullptr)
		events_ = std::make_shared<menu_event>(*this);
	return events_.get();
}

bool ewin::menu::object::has_events_() const{
	return (events_ != nullptr);
}

void ewin::menu::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	reflect.initialize_(nullptr, handler);
	is_forbidden.initialize_(nullptr, handler);

	app.initialize_(nullptr, handler);
	handle.initialize_(nullptr, handler);

	size.initialize_(&cache_.size, handler);
	position.initialize_(&cache_.position, handler);
	rect.initialize_(&cache_.rect, handler);

	created.initialize_(nullptr, handler);
	create.initialize_(nullptr, handler);
	auto_destroy.initialize_(&auto_destroy_, nullptr);

	events.initialize_(nullptr, handler);
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

	if (is_forbidden_(property_forbidden_info{ prop, access })){
		set_error_(error_type::forbidden_property);
		return;
	}

	error_value_ = error_type::nil;
	if (prop == &app){
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
	else if (prop == &events)
		*static_cast<menu_event **>(arg) = get_events_();
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
