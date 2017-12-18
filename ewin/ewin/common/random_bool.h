#pragma once

#ifndef EWIN_RANDOM_BOOL_H
#define EWIN_RANDOM_BOOL_H

#include "random_generator.h"

namespace ewin::common{
	template <template <class> class distribution_type = std::uniform_int_distribution, class engine_type = std::mt19937, class seeder_type = std::random_device>
	class basic_random_bool{
	public:
		template <typename... args_types>
		explicit basic_random_bool(args_types &&... args)
			: generator_(std::forward<args_types>(args)...){}

		bool operator ()(){
			return (generator_(0, 1) != 0);
		}

	private:
		random_generator<distribution_type, engine_type, seeder_type> generator_;
	};

	using random_bool = basic_random_bool<>;
}

#endif /* !EWIN_RANDOM_BOOL_H */
