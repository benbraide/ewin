#pragma once

#ifndef EWIN_ERROR_H
#define EWIN_ERROR_H

namespace ewin::common{
	enum class error_type{
		nil,
		uninitialized_property,
		property_access_violation,
		forbidden_property,
		no_app,
	};

	enum class error_throw_policy_type{
		always,
		once,
		never,
	};
}

#endif /* !EWIN_ERROR_H */
