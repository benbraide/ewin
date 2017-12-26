#pragma once

#ifndef EWIN_WINDOW_ITERATORS_H
#define EWIN_WINDOW_ITERATORS_H

namespace ewin::window{
	template <class object_type>
	class sibling_forward_iterator{
	public:
		sibling_forward_iterator()
			: value_(nullptr), target_(nullptr){}

		sibling_forward_iterator(object_type *value, object_type *target)
			: value_(value), target_(target){
			if (value_ == target_)//Advance
				value_ = value_->tree.next_sibling;
		}

		sibling_forward_iterator & operator++(){
			if (value_ != nullptr){
				value_ = value_->tree.next_sibling;
				if (value_ == target_)//Advance
					value_ = value_->tree.next_sibling;
			}
			else//Error
				throw common::error_type::bad_iterator;

			return *this;
		}

		const object_type &operator *() const{
			return *value_;
		}

		object_type &operator *(){
			return *value_;
		}

		bool operator ==(const sibling_forward_iterator &rhs) const{
			return (value_ == rhs.value_);
		}

		bool operator !=(const sibling_forward_iterator &rhs) const{
			return !(*this == rhs);
		}

	private:
		object_type *value_;
		object_type *target_;
	};

	template <class object_type>
	class ancestor_forward_iterator{
	public:
		ancestor_forward_iterator()
			: value_(nullptr){}

		explicit ancestor_forward_iterator(object_type *value)
			: value_(value){}

		ancestor_forward_iterator & operator++(){
			if (value_ != nullptr)
				value_ = value_->tree.parent;
			else//Error
				throw common::error_type::bad_iterator;

			return *this;
		}

		const object_type &operator *() const{
			return *value_;
		}

		object_type &operator *(){
			return *value_;
		}

		bool operator ==(const ancestor_forward_iterator &rhs) const{
			return (value_ == rhs.value_);
		}

		bool operator !=(const ancestor_forward_iterator &rhs) const{
			return !(*this == rhs);
		}

	private:
		object_type *value_;
	};
}

#endif /* !EWIN_WINDOW_ITERATORS_H */
