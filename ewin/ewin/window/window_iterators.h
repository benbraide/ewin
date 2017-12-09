#pragma once

#ifndef EWIN_WINDOW_ITERATORS_H
#define EWIN_WINDOW_ITERATORS_H

namespace ewin::window{
	class object;

	class sibling_forward_iterator{
	public:
		sibling_forward_iterator();

		sibling_forward_iterator(object *value, object *target);

		sibling_forward_iterator &operator ++();

		const object &operator *() const;

		object &operator *();

		bool operator ==(const sibling_forward_iterator &rhs) const;

		bool operator !=(const sibling_forward_iterator &rhs) const;

	private:
		object *value_;
		object *target_;
	};

	class ancestor_forward_iterator{
	public:
		ancestor_forward_iterator();

		explicit ancestor_forward_iterator(object *value);

		ancestor_forward_iterator &operator ++();

		const object &operator *() const;

		object &operator *();

		bool operator ==(const ancestor_forward_iterator &rhs) const;

		bool operator !=(const ancestor_forward_iterator &rhs) const;

	private:
		object *value_;
	};
}

#endif /* !EWIN_WINDOW_ITERATORS_H */
