#pragma once

#ifndef EWIN_ERROR_H
#define EWIN_ERROR_H

namespace ewin::common{
	enum class error_type{
		nil,
		uninitialized_property,
		property_access_violation,
		forbidden_property,
		bad_iterator,
		no_app,
		multiple_app,
		locked_app,
		parent_not_created,
	};

	enum class error_throw_policy_type{
		always,
		once,
		never,
	};
}

#endif /* !EWIN_ERROR_H */
