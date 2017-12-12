#pragma once

#ifndef EWIN_ERROR_H
#define EWIN_ERROR_H

namespace ewin::common{
	enum class error_type{
		nil,
		local_error,
		uninitialized_property,
		property_access_violation,
		forbidden_property,
		bad_iterator,
		no_app,
		multiple_app,
		locked_app,
		failed_to_retrieve_message,
		app_mismatch,
		cross_thread,
		window_not_created,
		parent_not_created,
		parent_change_forbidden,
		parent_required,
	};

	enum class error_throw_policy_type{
		always,
		once,
		never,
	};
}

#endif /* !EWIN_ERROR_H */
