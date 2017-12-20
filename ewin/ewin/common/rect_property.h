#pragma once

#ifndef EWIN_RECT_PROPERTY_H
#define EWIN_RECT_PROPERTY_H

#include "numeric_property.h"

namespace ewin::common{
	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class rect_value_property : public value_property<value_type, manager_type, access>{
	public:
		typedef value_type value_type;
		typedef decltype(value_type::left) backend_value_type;

		typedef value_property<value_type, manager_type, access> base_type;
		typedef numeric_value_property<backend_value_type, rect_value_property, access> numeric_value_property_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit rect_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(rect_value_property);

			left.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->left), handler);
			top.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->top), handler);
			right.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->right), handler);
			bottom.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->bottom), handler);
		}

		template <typename target_type>
		rect_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		template <typename target_type>
		bool operator ==(const target_type &rhs) const{
			return (*this == (value_type)rhs);
		}

		bool operator ==(const value_type &rhs) const{
			if (base_type::linked_ != nullptr)//Use linked value
				return (base_type::linked_->left == rhs.left && base_type::linked_->top == rhs.top && base_type::linked_->right == rhs.right && base_type::linked_->bottom == rhs.bottom);
			auto left = (value_type)(*this);
			return (left.left == rhs.left && left.top == rhs.top && left.right == rhs.right && left.bottom == rhs.bottom);
		}

		bool operator !=(const value_type &rhs) const{
			return !(*this == rhs);
		}

		template <typename target_type, typename unused_type = value_type>
		friend std::enable_if_t<!std::is_same_v<target_type, rect_value_property<unused_type, manager_type, access>>, bool> operator ==(const target_type &lhs, const rect_value_property &rhs){
			return (rhs == lhs);
		}

		template <typename target_type, typename unused_type = value_type>
		friend std::enable_if_t<!std::is_same_v<target_type, rect_value_property<unused_type, manager_type, access>>, bool> operator !=(const target_type &lhs, const rect_value_property &rhs){
			return (rhs != lhs);
		}

		numeric_value_property_type left;
		numeric_value_property_type top;
		numeric_value_property_type right;
		numeric_value_property_type bottom;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			base_type::initialize_(linked, callback);
			if (linked != nullptr){//Update linked
				auto handler = EWIN_PROP_HANDLER(rect_value_property);

				left.initialize_(((linked == nullptr) ? nullptr : &linked->left), handler);
				top.initialize_(((linked == nullptr) ? nullptr : &linked->top), handler);
				right.initialize_(((linked == nullptr) ? nullptr : &linked->right), handler);
				bottom.initialize_(((linked == nullptr) ? nullptr : &linked->bottom), handler);
			}
		}

		void handle_property_(void *prop, void *arg, property_access access){
			if (arg != nullptr && base_type::linked_ == nullptr){
				if (base_type::callback_ == nullptr)
					throw error_type::uninitialized_property;

				if (prop == &left){
					if (access == property_access::write){
						auto rect = operator value_type();
						auto value = value_type{ *reinterpret_cast<backend_value_type *>(arg), rect.top, rect.right, rect.bottom };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).left;
				}
				else if (prop == &top){
					if (access == property_access::write){
						auto rect = operator value_type();
						auto value = value_type{ rect.left, *reinterpret_cast<backend_value_type *>(arg), rect.right, rect.bottom };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).top;
				}
				else if (prop == &right){
					if (access == property_access::write){
						auto rect = operator value_type();
						auto value = value_type{ rect.left, rect.top, *reinterpret_cast<backend_value_type *>(arg), rect.bottom };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).right;
				}
				else if (prop == &bottom){
					if (access == property_access::write){
						auto rect = operator value_type();
						auto value = value_type{ rect.left, rect.top, rect.right, *reinterpret_cast<backend_value_type *>(arg) };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).bottom;
				}
			}
			else//Alert
				base_type::callback_(this, nullptr, access);
		}
	};

	template <class value_type, class manager_type = void>
	using read_only_rect_value_property = rect_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_rect_value_property = rect_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_RECT_PROPERTY_H */
