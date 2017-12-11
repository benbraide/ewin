#include "window_object.h"

ewin::window::wnd_tree::wnd_tree(object &target)
	: target_(&target){
	bind_properties_();
}

void ewin::window::wnd_tree::bind_properties_(){
	auto handler = std::bind(&wnd_tree::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	owner.initialize_(target_, nullptr);
	parent.initialize_(nullptr, handler);

	index.initialize_(nullptr, handler);
	absolute_index.initialize_(nullptr, handler);
	group_index.initialize_(nullptr, handler);

	previous_sibling.initialize_(nullptr, handler);
	next_sibling.initialize_(nullptr, handler);

	children.initialize_(nullptr, handler);
	siblings.initialize_(nullptr, handler);
	ancestors.initialize_(nullptr, handler);
}

void ewin::window::wnd_tree::handle_property_(void *prop, void *arg, common::property_access access){
	if (target_->is_forbidden[prop]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
	if (prop == &parent){
		if (access == common::property_access::write)
			set_parent_(*reinterpret_cast<object **>(arg), static_cast<std::size_t>(-1));
		else if (access == common::property_access::read)
			*reinterpret_cast<object **>(arg) = parent_.get();
	}
	else if (prop == &previous_sibling){
		if (access == common::property_access::write){
			auto value = *reinterpret_cast<object **>(arg);
			set_parent_(value->tree.parent, (value->tree.index + 1u));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<object **>(arg) = sibling_(true);
	}
	else if (prop == &next_sibling){
		if (access == common::property_access::write){
			auto value = *reinterpret_cast<object **>(arg);
			set_parent_(value->tree.parent, value->tree.index);
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<object **>(arg) = sibling_(false);
	}
	else if (prop == &children){
		if (access == common::property_access::list_at){
			auto &info = *reinterpret_cast<std::pair<std::size_t, object *> *>(arg);
			info.second = at_(info.first, list_target_type::children);
		}
		else if (access == common::property_access::list_find){
			auto &info = *reinterpret_cast<std::pair<std::size_t, object *> *>(arg);
			info.first = find_(*info.second, list_target_type::children);
		}
		else if (access == common::property_access::list_size)
			*reinterpret_cast<std::size_t *>(arg) = children_.size();
		else if (access == common::property_access::list_begin)
			*reinterpret_cast<object_list_iterator_type *>(arg) = children_.begin();
		else if (access == common::property_access::list_end)
			*reinterpret_cast<object_list_iterator_type *>(arg) = children_.end();
		else if (access == common::property_access::list_add)
			add_child_(reinterpret_cast<object *>(arg));
		else if (access == common::property_access::list_remove)
			remove_child_(reinterpret_cast<object *>(arg));
		else if (access == common::property_access::list_remove_index)
			remove_child_(*reinterpret_cast<std::size_t *>(arg));
	}
	else if (prop == &siblings){
		if (access == common::property_access::list_at){
			auto &info = *reinterpret_cast<std::pair<std::size_t, object *> *>(arg);
			info.second = at_(info.first, list_target_type::siblings);
		}
		else if (access == common::property_access::list_find){
			auto &info = *reinterpret_cast<std::pair<std::size_t, object *> *>(arg);
			info.first = find_(*info.second, list_target_type::siblings);
		}
		else if (access == common::property_access::list_size)
			*reinterpret_cast<std::size_t *>(arg) = (target_->tree.parent_->tree.children_.size() - 1u);
		else if (access == common::property_access::list_begin)
			*reinterpret_cast<sibling_forward_iterator *>(arg) = sibling_forward_iterator(target_->tree.parent_->tree.children_.begin()->get(), target_);
		else if (access == common::property_access::list_end)
			*reinterpret_cast<sibling_forward_iterator *>(arg) = sibling_forward_iterator();
	}
	else if (prop == &ancestors){
		if (access == common::property_access::list_at){
			auto &info = *reinterpret_cast<std::pair<std::size_t, object *> *>(arg);
			info.second = at_(info.first, list_target_type::ancestors);
		}
		else if (access == common::property_access::list_find){
			auto &info = *reinterpret_cast<std::pair<std::size_t, object *> *>(arg);
			info.first = find_(*info.second, list_target_type::ancestors);
		}
		else if (access == common::property_access::list_size)
			*reinterpret_cast<std::size_t *>(arg) = ancestor_count_();
		else if (access == common::property_access::list_begin)
			*reinterpret_cast<ancestor_forward_iterator *>(arg) = ancestor_forward_iterator(target_->tree.parent_.get());
		else if (access == common::property_access::list_end)
			*reinterpret_cast<ancestor_forward_iterator *>(arg) = ancestor_forward_iterator();
	}
}

void ewin::window::wnd_tree::set_parent_(object *value, std::size_t index){
	if (parent_.get() == value){//Same value
		if (value == nullptr)
			return;//No changes

		auto current_index = index_(false);
		if (index == current_index || (index >= parent_->tree.children_.size() && current_index == (parent_->tree.children_.size() - 1u)))
			return;//No changes
	}

	if (target_->attribute.is_message_only){//Cannot change parent
		target_->error = common::error_type::parent_change_forbidden;
		return;
	}

	if (target_->attribute.is_control && value == nullptr){//Parent required
		target_->error = common::error_type::parent_required;
		return;
	}

	if (value != nullptr && target_->handle != nullptr){//Target window has been created
		if (value->handle == nullptr){
			target_->error = common::error_type::parent_not_created;
			return;
		}

		if (value->app != target_->app){//Cannot move to parent in a different app
			target_->error = common::error_type::app_mismatch;
			return;
		}
	}

	auto &parent_children = parent_->tree.children_;
	object_ptr_type target = target_->reflect;
	auto old_parent = parent_;

	target_->error = common::error_type::nil;
	if (parent_ != nullptr){//Remove from current parent
		auto iter = std::find(parent_children.begin(), parent_children.end(), target);
		if (iter != parent_children.end()){
			auto old_index = std::distance(parent_children.begin(), iter);
			parent_children.erase(iter);
			parent_->changed = object::child_change_info{ target_, static_cast<std::size_t>(old_index), true };
		}
	}

	if ((parent_ = value->reflect) != nullptr){//Insert into list
		if (index < parent_children.size())
			parent_children.insert((std::next(parent_children.begin(), index)), target);
		else//Append
			parent_children.push_back(target);
		parent_->changed = object::child_change_info{ target_, ((index < parent_children.size()) ? index : (parent_children.size() - 1u)), false };
	}

	if (target_->handle != nullptr){//Update
		::SetParent(target_->handle, ((parent_ == nullptr) ? nullptr : static_cast<common::types::hwnd>(parent_->handle)));
		if (old_parent == nullptr && parent_ != nullptr)
			target_->style.value += WS_CHILD;//Set child style
		else if (old_parent != nullptr && parent_ == nullptr)
			target_->style.value -= WS_CHILD;//Remove child style
	}

	target_->changed = object::parent_change_info{ old_parent.get(), parent_.get() };
}

std::size_t ewin::window::wnd_tree::index_(bool absolute) const{
	if (parent_ == nullptr)//No parent
		return static_cast<std::size_t>(-1);

	object_ptr_type target = target_->reflect;
	auto value = std::distance(parent_->tree.children_.begin(), std::find(parent_->tree.children_.begin(), parent_->tree.children_.end(), target));

	return (absolute ? (parent_->tree.group_index + value) : value);
}

ewin::window::object *ewin::window::wnd_tree::sibling_(bool previous) const{
	if (parent_ == nullptr || parent_->tree.children_.size() < 2u)//No sibling
		return nullptr;

	auto &parent_children = parent_->tree.children_;
	auto end_iter = parent_children.end();

	if (previous){
		for (auto prev = parent_children.begin(), iter = std::next(prev); iter != end_iter; ++prev, ++iter){
			if (iter->get() == target_)//Match
				return prev->get();
		}
	}
	else{//Next
		for (auto iter = parent_children.begin(); iter != end_iter; ++iter){
			if (iter->get() == target_)//Match
				return ((++iter == end_iter) ? nullptr : iter->get());
		}
	}

	return nullptr;
}

ewin::window::object *ewin::window::wnd_tree::ancestor_(std::size_t index) const{
	auto ancestor = parent_;
	for (; index > 0u && ancestor != nullptr; --index)
		ancestor = ancestor->tree.parent_;
	return ancestor.get();
}

ewin::window::object *ewin::window::wnd_tree::at_(std::size_t index, list_target_type list_target) const{
	if (list_target == list_target_type::ancestors)
		return ancestor_(index);

	if (list_target == list_target_type::children)
		return ((index < children_.size()) ? std::next(children_.begin(), index)->get() : nullptr);

	if (target_->tree.parent_->tree.children_.size() < 2u)
		return nullptr;//No sibling

	auto sibling = target_->tree.parent_->tree.children_.begin()->get();
	if (sibling == target_)//Advance
		sibling = sibling->tree.next_sibling;

	for (; sibling != nullptr && index > 0u; --index){//Traverse siblings
		sibling = target_->tree.parent_->tree.children_.begin()->get();
		if (sibling == target_)//Advance
			sibling = sibling->tree.next_sibling;
	}

	return sibling;
}

std::size_t ewin::window::wnd_tree::find_(object &target, list_target_type list_target) const{
	if (list_target == list_target_type::children){//Find target in children list
		auto iter = std::find(children_.begin(), children_.end(), static_cast<object_ptr_type>(target.reflect));
		return ((iter == children_.end()) ? static_cast<std::size_t>(-1) : static_cast<std::size_t>(std::distance(children_.begin(), iter)));
	}

	std::size_t index = 0u;
	if (list_target == list_target_type::siblings){//Find target in sibling list
		auto sibling = target_->tree.parent_->tree.children_.begin()->get();
		if (sibling == target_)//Advance
			sibling = sibling->tree.next_sibling;

		for (; sibling != nullptr && sibling != &target; ++index){//Traverse siblings
			sibling = target_->tree.parent_->tree.children_.begin()->get();
			if (sibling == target_)//Advance
				sibling = sibling->tree.next_sibling;
		}
	}
	else{//Find target in ancestor list
		auto ancestor = parent_;
		for (; ancestor != nullptr && ancestor.get() != &target; ++index)
			ancestor = ancestor->tree.parent_;
	}

	return index;
}

std::size_t ewin::window::wnd_tree::ancestor_count_() const{
	std::size_t count = 0u;
	for (auto ancestor = parent_; ancestor != nullptr; ++count)
		ancestor = ancestor->tree.parent_;
	return count;
}

void ewin::window::wnd_tree::add_child_(object *value){
	if (value->tree.parent_.get() != target_)
		value->tree.set_parent_(target_, children_.size());
}

void ewin::window::wnd_tree::remove_child_(object *value){
	if (value->tree.parent_.get() == target_)
		value->tree.set_parent_(nullptr, 0u);
}

void ewin::window::wnd_tree::remove_child_(std::size_t index){
	if (index < children_.size())
		remove_child_(std::next(children_.begin(), index)->get());
}
