#pragma once

#ifndef EWIN_RANDOM_NUMBER_H
#define EWIN_RANDOM_NUMBER_H

#include "random_generator.h"

namespace ewin::common{
	template <template <class> class distribution_type = std::uniform_int_distribution, class engine_type = std::mt19937, class seeder_type = std::random_device>
	class random_number : public random_generator<distribution_type, engine_type, seeder_type>{
	public:
		typedef random_generator<distribution_type, engine_type, seeder_type> base_type;

		template <typename... args_types>
		explicit random_number(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		template <typename target_type>
		target_type operator ()(target_type from, target_type to){
			return base_type::operator ()(from, to);
		}

		template <typename target_type>
		target_type operator ()(target_type from_zero_to){
			return base_type::operator ()(static_cast<target_type>(0), from_zero_to);
		}
	};

	using random_integral_number	= random_number<>;
	using random_real_number		= random_number<std::uniform_real_distribution>;
}

#endif /* !EWIN_RANDOM_NUMBER_H */
