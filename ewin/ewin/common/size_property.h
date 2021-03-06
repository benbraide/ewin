#pragma once

#ifndef EWIN_SIZE_PROPERTY_H
#define EWIN_SIZE_PROPERTY_H

#include "numeric_property.h"

namespace ewin::common{
	namespace detail{
		template <class type, class unused_type = void>
		struct has_cx : std::false_type{};

		template <class type>
		struct has_cx<type, decltype((void)type::cx)> : std::true_type{};
	}

	template <class value_type, class manager_type, property_access access, bool has_cx>
	class basic_size_value_property : public value_property<value_type, manager_type, access>{
	public:
		typedef value_type value_type;
		typedef decltype(value_type::cx) backend_value_type;

		typedef value_property<value_type, manager_type, access> base_type;
		typedef numeric_value_property<backend_value_type, basic_size_value_property, access> numeric_value_property_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit basic_size_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(basic_size_value_property);
			if (base_type::callback_ == nullptr){
				width.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->cx), nullptr);
				height.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->cy), nullptr);
			}
			else{//Bind callback
				width.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->cx), handler);
				height.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->cy), handler);
			}
		}

		template <typename target_type>
		basic_size_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		template <typename target_type>
		bool operator ==(const target_type &rhs) const{
			return (*this == (value_type)rhs);
		}

		bool operator ==(const value_type &rhs) const{
			if (base_type::linked_ != nullptr)//Use linked value
				return (base_type::linked_->cx == rhs.cx && base_type::linked_->cy == rhs.cy);
			auto left = (value_type)(*this);
			return (left.cx == rhs.cx && left.cy == rhs.cy);
		}

		bool operator !=(const value_type &rhs) const{
			return !(*this == rhs);
		}

		numeric_value_property_type width;
		numeric_value_property_type height;

		EWIN_EX_PROP_FRIEND_OPCOMP(basic_size_value_property, has_cx)

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			base_type::initialize_(linked, callback);
			auto handler = EWIN_PROP_HANDLER(basic_size_value_property);
			if (base_type::callback_ == nullptr){
				width.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->cx), nullptr);
				height.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->cy), nullptr);
			}
			else{//Bind callback
				width.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->cx), handler);
				height.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->cy), handler);
			}
		}

		void handle_property_(void *prop, void *arg, property_access access){
			if (arg != nullptr && base_type::linked_ == nullptr){
				if (base_type::callback_ == nullptr)
					throw error_type::uninitialized_property;

				if (prop == &width){
					if (access == property_access::write){
						auto value = value_type{ *reinterpret_cast<backend_value_type *>(arg), (operator value_type()).cy };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).cx;
				}
				else if (prop == &height){
					if (access == property_access::write){
						auto value = value_type{ (operator value_type()).cx, *reinterpret_cast<backend_value_type *>(arg) };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).cy;
				}
			}
			else//Alert
				base_type::callback_(this, nullptr, access);
		}
	};

	template <class value_type, class manager_type, property_access access>
	class basic_size_value_property<value_type, manager_type, access, false> : public value_property<value_type, manager_type, access>{
	public:
		typedef value_type value_type;
		typedef decltype(value_type::width) backend_value_type;

		typedef value_property<value_type, manager_type, access> base_type;
		typedef numeric_value_property<backend_value_type, basic_size_value_property, access> numeric_value_property_type;

		using base_type::operator value_type;

		template <typename... args_types>
		explicit basic_size_value_property(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(basic_size_value_property);
			if (base_type::callback_ == nullptr){
				width.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->width), nullptr);
				height.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->height), nullptr);
			}
			else{//Bind callback
				width.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->width), handler);
				height.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->height), handler);
			}
		}

		template <typename target_type>
		basic_size_value_property &operator =(const target_type &value){
			base_type::operator =(value);
			return *this;
		}

		template <typename target_type>
		bool operator ==(const target_type &rhs) const{
			return (*this == (value_type)rhs);
		}

		bool operator ==(const value_type &rhs) const{
			if (base_type::linked_ != nullptr)//Use linked value
				return (base_type::linked_->width == rhs.width && base_type::linked_->height == rhs.height);
			auto left = (value_type)(*this);
			return (left.width == rhs.width && left.height == rhs.height);
		}

		bool operator !=(const value_type &rhs) const{
			return !(*this == rhs);
		}

		numeric_value_property_type width;
		numeric_value_property_type height;

		EWIN_EX_PROP_FRIEND_OPCOMP(basic_size_value_property, false)

	protected:
		friend class property_manager;
		friend std::conditional_t<std::is_void_v<manager_type>, value_property, manager_type>;

		void initialize_(value_type *linked, callback_type callback){
			base_type::initialize_(linked, callback);
			auto handler = EWIN_PROP_HANDLER(basic_size_value_property);
			if (base_type::callback_ == nullptr){
				width.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->width), nullptr);
				height.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->height), nullptr);
			}
			else{//Bind callback
				width.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->width), handler);
				height.initialize_(((base_type::linked_ == nullptr) ? nullptr : &base_type::linked_->height), handler);
			}
		}

		void handle_property_(void *prop, void *arg, property_access access){
			if (arg != nullptr && base_type::linked_ == nullptr){
				if (base_type::callback_ == nullptr)
					throw error_type::uninitialized_property;

				if (prop == &width){
					if (access == property_access::write){
						auto value = value_type{ *reinterpret_cast<backend_value_type *>(arg), (operator value_type()).height };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).width;
				}
				else if (prop == &height){
					if (access == property_access::write){
						auto value = value_type{ (operator value_type()).width, *reinterpret_cast<backend_value_type *>(arg) };
						base_type::callback_(this, &value, access);
					}
					else if (access == property_access::read)
						*reinterpret_cast<backend_value_type *>(arg) = (operator value_type()).height;
				}
			}
			else//Alert
				base_type::callback_(this, nullptr, access);
		}
	};

	template <class value_type, class manager_type = void, property_access access = property_access::nil>
	using size_value_property = basic_size_value_property<value_type, manager_type, access, detail::has_cx<value_type>::value>;

	template <class value_type, class manager_type = void>
	using read_only_size_value_property = size_value_property<value_type, manager_type, property_access::read>;

	template <class value_type, class manager_type = void>
	using write_only_size_value_property = size_value_property<value_type, manager_type, property_access::write>;
}

#endif /* !EWIN_SIZE_PROPERTY_H */
