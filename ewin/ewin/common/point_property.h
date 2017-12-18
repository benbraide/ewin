#pragma once

#ifndef EWIN_POINT_PROPERTY_H
#define EWIN_POINT_PROPERTY_H

#include "numeric_property.h"

namespace ewin::common{
	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	class point_value_property : public value_property<value_type, manager_type, access>{
	public:
		typedef value_type value_type;
		typedef decltype(value_type::x) backend_value_type;

		typedef value_property<value_type, manager_type, access> base_type;
		typedef numeric_value_property<backend_value_type, point_value_property, access> numeric_value_property_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit point_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(point_value_property);
			x.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->x), handler);
			y.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->y), handler);
		}

		template <typename target_type>
		point_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		template <typename target_type>
		bool operator ==(const target_type &rhs) const{
			return (*this == (value_type)rhs);
		}

		bool operator ==(const value_type &rhs) const{
			if (base_type::linked_ != nullptr)//Use linked value
				return (base_type::linked_->x == rhs.x && base_type::linked_->y == rhs.y);
			auto left = (value_type)(*this);
			return (left.x == rhs.x && left.y == rhs.y);
		}

		bool operator !=(const value_type &rhs) const{
			return !(*this == rhs);
		}

		template <typename target_type, typename unused_type = value_type>
		friend std::enable_if_t<!std::is_same_v<target_type, point_value_property>, bool> operator ==(const target_type &lhs, const point_value_property &rhs){
			return (rhs == lhs);
		}

		template <typename target_type, typename unused_type = value_type>
		friend std::enable_if_t<!std::is_same_v<target_type, point_value_property>, bool> operator !=(const target_type &lhs, const point_value_property &rhs){
			return (rhs != lhs);
		}

		numeric_value_property_type x;
		numeric_value_property_type y;

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			base_type::initialize_(linked, callback);
			if (linked != nullptr){//Update linked
				auto handler = EWIN_PROP_HANDLER(point_value_property);
				x.initialize_(&linked->x, handler);
				y.initialize_(&linked->y, handler);
			}
		}

		void handle_property_(void *prop, void *arg, property_access access){
			if (arg != nullptr && base_type::linked_ == nullptr){
				if (base_type::callback_ == nullptr)
					throw error_type::uninitialized_property;

				if (prop == &x){
					if (access == property_access::write){
						auto value = value_type{ *reinterpret_cast<backend_value_type *>(arg), (operator value_type()).y };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).x;	
				}
				else if (prop == &y){
					if (access == property_access::write){
						auto value = value_type{ (operator value_type()).x, *reinterpret_cast<backend_value_type *>(arg) };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).y;
				}
			}
			else//Alert
				base_type::callback_(this, nullptr, access);
		}
	};

	template <class value_type, class manager_type = void>
	using read_only_point_value_property = point_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_point_value_property = point_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_POINT_PROPERTY_H */
