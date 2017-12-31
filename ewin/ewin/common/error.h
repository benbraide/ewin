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
		no_main_app,
		multiple_app,
		multiple_main_app,
		locked_app,
		failed_to_retrieve_message,
		app_mismatch,
		cross_thread,
		menu_not_created,
		bad_track_target,
		window_not_created,
		parent_not_created,
		parent_not_found,
		parent_change_forbidden,
		parent_required,
	};

	enum class error_throw_policy_type{
		always,
		once,
		no_throw_once,
		never,
	};
}

#endif /* !EWIN_ERROR_H */
