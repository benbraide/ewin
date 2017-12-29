#pragma once

#ifndef EWIN_WINDOW_TREE_H
#define EWIN_WINDOW_TREE_H

#include <list>
#include <memory>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/list_property.h"
#include "../common/object_property.h"

#include "window_iterators.h"

namespace ewin::window{
	template <class object_type>
	class wnd_tree{
	public:
		typedef object_type object_type;

		typedef std::shared_ptr<object_type> object_ptr_type;
		typedef std::list<object_type *> object_list_type;

		typedef typename object_list_type::iterator object_list_iterator_type;
		typedef typename object_type::property_forbidden_info property_forbidden_info;

		enum class list_target_type{
			children,
			siblings,
			ancestors,
		};

		explicit wnd_tree(object_type &target)
			: target_(&target){
			bind_properties_();
		}

		wnd_tree(const wnd_tree &) = delete;
		
		wnd_tree &operator =(const wnd_tree &) = delete;

		common::read_only_object_value_property<object_type, wnd_tree> owner;
		common::object_value_property<object_type, wnd_tree> parent;

		common::numeric_value_property<std::size_t, wnd_tree> index;
		common::object_value_property<object_type, wnd_tree> previous_sibling;
		common::object_value_property<object_type, wnd_tree> next_sibling;

		common::list_value_property<object_type, object_list_iterator_type, object_list_iterator_type, wnd_tree> children;
		common::read_only_list_value_property<object_type, sibling_forward_iterator<object_type>, sibling_forward_iterator<object_type>, wnd_tree> siblings;
		common::read_only_list_value_property<object_type, ancestor_forward_iterator<object_type>, ancestor_forward_iterator<object_type>, wnd_tree> ancestors;

	private:
		friend object_type;

		void bind_properties_(){
			auto handler = EWIN_PROP_HANDLER(wnd_tree);

			owner.initialize_(target_, nullptr);
			parent.initialize_(nullptr, handler);

			index.initialize_(nullptr, handler);
			previous_sibling.initialize_(nullptr, handler);
			next_sibling.initialize_(nullptr, handler);

			children.initialize_(nullptr, handler);
			siblings.initialize_(nullptr, handler);
			ancestors.initialize_(nullptr, handler);
		}

		void handle_property_(void *prop, void *arg, common::property_access access){
			if (target_->is_forbidden[property_forbidden_info{ prop, access }]){
				target_->set_error_(common::error_type::forbidden_property);
				return;
			}

			target_->set_error_(common::error_type::nil);//Clear error
			if (prop == &parent){
				if (access == common::property_access::write)
					set_parent_(reinterpret_cast<object_type *>(arg), static_cast<std::size_t>(-1));
				else if (access == common::property_access::read)
					*reinterpret_cast<object_type **>(arg) = parent_;
			}
			else if (prop == &index){
				if (access == common::property_access::write)
					set_parent_(parent_, *reinterpret_cast<std::size_t *>(arg));
				else if (access == common::property_access::read)
					*reinterpret_cast<std::size_t *>(arg) = index_();
			}
			else if (prop == &previous_sibling){
				if (access == common::property_access::write){
					auto value = reinterpret_cast<object_type *>(arg);
					set_parent_(value->tree.parent, (value->tree.index_() + 1u));
				}
				else if (access == common::property_access::read)
					*reinterpret_cast<object_type **>(arg) = sibling_(true);
			}
			else if (prop == &next_sibling){
				if (access == common::property_access::write){
					auto value = reinterpret_cast<object_type *>(arg);
					set_parent_(value->tree.parent, value->tree.index_());
				}
				else if (access == common::property_access::read)
					*reinterpret_cast<object_type **>(arg) = sibling_(false);
			}
			else if (prop == &children){
				if (access == common::property_access::list_add){
					auto &info = *reinterpret_cast<std::pair<std::size_t, object_type *> *>(arg);
					info.first = add_child_(info.second);
				}
				else if (access == common::property_access::list_at){
					auto &info = *reinterpret_cast<std::pair<std::size_t, object_type *> *>(arg);
					info.second = at_(info.first, list_target_type::children);
				}
				else if (access == common::property_access::list_find){
					auto &info = *reinterpret_cast<std::pair<std::size_t, object_type *> *>(arg);
					info.first = find_(*info.second, list_target_type::children);
				}
				else if (access == common::property_access::list_size)
					*reinterpret_cast<std::size_t *>(arg) = children_.size();
				else if (access == common::property_access::list_begin)
					*reinterpret_cast<object_list_iterator_type *>(arg) = children_.begin();
				else if (access == common::property_access::list_end)
					*reinterpret_cast<object_list_iterator_type *>(arg) = children_.end();
				else if (access == common::property_access::list_remove)
					remove_child_(reinterpret_cast<object_type *>(arg));
				else if (access == common::property_access::list_remove_index)
					remove_child_(*reinterpret_cast<std::size_t *>(arg));
			}
			else if (prop == &siblings){
				if (access == common::property_access::list_at){
					auto &info = *reinterpret_cast<std::pair<std::size_t, object_type *> *>(arg);
					info.second = at_(info.first, list_target_type::siblings);
				}
				else if (access == common::property_access::list_find){
					auto &info = *reinterpret_cast<std::pair<std::size_t, object_type *> *>(arg);
					info.first = find_(*info.second, list_target_type::siblings);
				}
				else if (access == common::property_access::list_size)
					*reinterpret_cast<std::size_t *>(arg) = (parent_->tree.children_.size() - 1u);
				else if (access == common::property_access::list_begin)
					*reinterpret_cast<sibling_forward_iterator<object_type> *>(arg) = sibling_forward_iterator<object_type>(*parent_->tree.children_.begin(), target_);
				else if (access == common::property_access::list_end)
					*reinterpret_cast<sibling_forward_iterator<object_type> *>(arg) = sibling_forward_iterator<object_type>();
			}
			else if (prop == &ancestors){
				if (access == common::property_access::list_at){
					auto &info = *reinterpret_cast<std::pair<std::size_t, object_type *> *>(arg);
					info.second = at_(info.first, list_target_type::ancestors);
				}
				else if (access == common::property_access::list_find){
					auto &info = *reinterpret_cast<std::pair<std::size_t, object_type *> *>(arg);
					info.first = find_(*info.second, list_target_type::ancestors);
				}
				else if (access == common::property_access::list_size)
					*reinterpret_cast<std::size_t *>(arg) = ancestor_count_();
				else if (access == common::property_access::list_begin)
					*reinterpret_cast<ancestor_forward_iterator<object_type> *>(arg) = ancestor_forward_iterator<object_type>(parent_);
				else if (access == common::property_access::list_end)
					*reinterpret_cast<ancestor_forward_iterator<object_type> *>(arg) = ancestor_forward_iterator<object_type>();
			}
		}

		std::size_t set_parent_(object_type *value, std::size_t index){
			if (parent_ == value){//Same value
				if (value == nullptr)
					return -1;//No changes

				auto current_index = index_();
				if (index == current_index || (index >= parent_->tree.children_.size() && current_index == (parent_->tree.children_.size() - 1u)))
					return -1;//No changes
			}

			if (value != nullptr){
				if (value->tree.parent_ == target_){//Error
					target_->set_error_(common::error_type::parent_change_forbidden);
					return -1;
				}

				if (value->created && target_->created && value->app != target_->app){
					target_->set_error_(common::error_type::app_mismatch);
					return -1;
				}
			}

			if (!target_->validate_parent_change_(value))
				return -1;//Parent change rejected

			if (parent_ != nullptr && !parent_->validate_child_remove_(*target_))
				return -1;//Target locked to current parent

			if (value != nullptr && !value->validate_child_add_(*target_, index))
				return -1;//Child insertion rejected by new parent

			auto old_parent = parent_;
			auto old_index = index_();

			if (parent_ != nullptr && !parent_->tree.children_.empty()){//Remove from current parent
				auto &parent_children = parent_->tree.children_;
				auto iter = std::find(parent_children.begin(), parent_children.end(), target_);
				if (iter != parent_children.end()){
					auto old_index = std::distance(parent_children.begin(), iter);
					parent_children.erase(iter);
					parent_->child_removed_(*target_, old_index);
				}
			}

			if ((parent_ = value) != nullptr){//Insert into list
				auto &parent_children = parent_->tree.children_;
				if (index >= parent_children.size()){//Append
					parent_children.push_back(target_);
					index = (parent_children.size() - 1u);
				}
				else//Insert
					parent_children.insert((std::next(parent_children.begin(), index)), target_);
				parent_->child_added_(*target_, index);
			}

			target_->parent_changed_(parent_, old_parent, index, old_index);
			return index;
		}

		std::size_t index_() const{
			return ((parent_ == nullptr) ? -1 : parent_->tree.find_(*target_, list_target_type::children));
		}

		object_type *sibling_(bool previous) const{
			if (parent_ == nullptr || parent_->tree.children_.size() < 2u)//No sibling
				return nullptr;

			auto &parent_children = parent_->tree.children_;
			auto end_iter = parent_children.end();

			if (previous){
				for (auto prev = parent_children.begin(), iter = std::next(prev); iter != end_iter; ++prev, ++iter){
					if (*iter == target_)//Match
						return *prev;
				}
			}
			else{//Next
				for (auto iter = parent_children.begin(); iter != end_iter; ++iter){
					if (*iter == target_)//Match
						return ((++iter == end_iter) ? nullptr : *iter);
				}
			}

			return nullptr;
		}

		object_type *ancestor_(std::size_t index) const{
			auto ancestor = parent_;
			for (; index > 0u && ancestor != nullptr; --index)
				ancestor = ancestor->tree.parent_;
			return ancestor;
		}

		object_type *at_(std::size_t index, list_target_type list_target) const{
			if (list_target == list_target_type::ancestors)
				return ancestor_(index);

			if (list_target == list_target_type::children)
				return ((index < children_.size()) ? *std::next(children_.begin(), index) : nullptr);

			auto &parent_children = parent_->tree.children_;
			for (auto sibling : parent_children){
				if (sibling != target_ && index == 0u)
					return sibling;//Match found
				--index;//Update
			}

			return nullptr;
		}

		std::size_t find_(object_type &target, list_target_type list_target) const{
			if (list_target == list_target_type::children){//Find target in children list
				auto iter = std::find(children_.begin(), children_.end(), &target);
				return ((iter == children_.end()) ? -1 : std::distance(children_.begin(), iter));
			}

			if (list_target == list_target_type::siblings){//Find target in sibling list
				std::size_t index = 0u;
				auto &parent_children = parent_->tree.children_;

				for (auto sibling : parent_children){
					if (sibling != target_ && sibling == &target)
						return index;//Match found
					++index;//Update
				}

				return -1;
			}

			std::size_t index = 0u;
			auto ancestor = parent_;
			for (; ancestor != nullptr && ancestor != &target; ++index)
				ancestor = ancestor->tree.parent_;

			return index;
		}

		std::size_t ancestor_count_() const{
			std::size_t count = 0u;
			for (auto ancestor = parent_; ancestor != nullptr; ++count)
				ancestor = ancestor->tree.parent_;
			return count;
		}

		std::size_t add_child_(object_type *value){
			return ((value->tree.parent_ == target_) ? value->tree.index_() : value->tree.set_parent_(target_, children_.size()));
		}

		void remove_child_(object_type *value){
			if (value->tree.parent_ == target_)
				value->tree.set_parent_(nullptr, 0u);
		}

		void remove_child_(std::size_t index){
			if (index < children_.size())
				remove_child_(*std::next(children_.begin(), index));
		}

		object_type *target_;
		object_type *parent_;
		object_list_type children_;
	};
}

#endif /* !EWIN_WINDOW_TREE_H */
