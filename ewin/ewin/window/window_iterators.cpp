#include "window_object.h"

ewin::window::sibling_forward_iterator::sibling_forward_iterator()
	: value_(nullptr), target_(nullptr){}

ewin::window::sibling_forward_iterator::sibling_forward_iterator(object *value, object *target)
	: value_(value), target_(target){
	if (value_ == target_)//Advance
		value_ = value_->tree->next_sibling;
}

ewin::window::sibling_forward_iterator &ewin::window::sibling_forward_iterator::operator++(){
	if (value_ != nullptr){
		value_ = value_->tree->next_sibling;
		if (value_ == target_)//Advance
			value_ = value_->tree->next_sibling;
	}
	else//Error
		throw common::error_type::bad_iterator;

	return *this;
}

const ewin::window::object &ewin::window::sibling_forward_iterator::operator*() const{
	return *value_;
}

ewin::window::object &ewin::window::sibling_forward_iterator::operator*(){
	return *value_;
}

bool ewin::window::sibling_forward_iterator::operator==(const sibling_forward_iterator &rhs) const{
	return (value_ == rhs.value_);
}

bool ewin::window::sibling_forward_iterator::operator!=(const sibling_forward_iterator &rhs) const{
	return !(*this == rhs);
}

ewin::window::ancestor_forward_iterator::ancestor_forward_iterator()
	: value_(nullptr){}

ewin::window::ancestor_forward_iterator::ancestor_forward_iterator(object *value)
	: value_(value){}

ewin::window::ancestor_forward_iterator &ewin::window::ancestor_forward_iterator::operator++(){
	if (value_ != nullptr)
		value_ = value_->tree->parent;
	else//Error
		throw common::error_type::bad_iterator;

	return *this;
}

const ewin::window::object &ewin::window::ancestor_forward_iterator::operator*() const{
	return *value_;
}

ewin::window::object &ewin::window::ancestor_forward_iterator::operator*(){
	return *value_;
}

bool ewin::window::ancestor_forward_iterator::operator==(const ancestor_forward_iterator &rhs) const{
	return (value_ == rhs.value_);
}

bool ewin::window::ancestor_forward_iterator::operator!=(const ancestor_forward_iterator &rhs) const{
	return !(*this == rhs);
}
