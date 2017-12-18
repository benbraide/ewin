#pragma once

#ifndef EWIN_RANDOM_STRING_H
#define EWIN_RANDOM_STRING_H

#include <string>
#include <vector>
#include <climits>
#include <unordered_map>

#include "macro.h"
#include "random_generator.h"

namespace ewin::common{
	enum class random_string_char_set : unsigned int{
		nil					= (0 << 0x0000),
		alpha				= (1 << 0x0000),
		uppercase			= (1 << 0x0001),
		lowercase			= (1 << 0x0002),
		digits				= (1 << 0x0003),
		oct					= (1 << 0x0004),
		hex					= (1 << 0x0005),
		invert				= (1 << 0x0006),
	};

	EWIN_MAKE_OPERATORS(random_string_char_set);

	template <template <class> class distribution_type = std::uniform_int_distribution, class engine_type = std::mt19937, class seeder_type = std::random_device>
	class basic_random_string{
	public:
		struct set_range{
			int from;
			int to;
		};

		typedef std::vector<set_range> set_range_list_type;
		typedef random_string_char_set char_set_type;

		template <typename... args_types>
		explicit basic_random_string(args_types &&... args)
			: generator_(std::forward<args_types>(args)...){}

		template <typename string_type>
		string_type generate(std::size_t min_size, std::size_t max_size, const set_range_list_type &char_set){
			return generate<string_type>(min_size, max_size, char_set, std::unordered_map<std::size_t, typename string_type::traits_type::char_type>{});
		}

		template <typename string_type>
		string_type generate(std::size_t min_size, std::size_t max_size, const set_range_list_type &char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return generate<string_type>(generator_(min_size, max_size), char_set, delim);
		}

		template <typename string_type>
		string_type generate(std::size_t size, const set_range_list_type &char_set){
			return generate<string_type>(size, char_set, std::unordered_map<std::size_t, typename string_type::traits_type::char_type>{});
		}

		template <typename string_type>
		string_type generate(std::size_t size, const set_range_list_type &char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			typedef typename string_type::traits_type::char_type char_type;
			typedef typename std::unordered_map<std::size_t, typename string_type::traits_type::char_type>::const_iterator const_iterator_type;

			if (size == 0u || char_set.empty())
				return string_type();

			string_type value;
			const_iterator_type iter;
			std::size_t char_set_index, max_char_set_index = (char_set.size() - 1u);

			value.reserve(size + delim.size());
			if (!delim.empty() && (iter = delim.find(static_cast<std::size_t>(0))) != delim.end())
				value.append(1, iter->second);//Append delimiter

			for (std::size_t count = 1; count <= size; ++count){
				char_set_index = ((max_char_set_index == 0u) ? 0u : generator_(static_cast<std::size_t>(0), max_char_set_index));
				value.append(1, static_cast<char_type>(generator_(char_set[char_set_index].from, char_set[char_set_index].to)));
				if (!delim.empty() && (iter = delim.find(count)) != delim.end())
					value.append(1, iter->second);//Append delimiter
			}

			return value;
		}

		template <typename string_type>
		string_type generate(std::size_t min_size, std::size_t max_size, char_set_type char_set){
			return generate<string_type>(min_size, max_size, char_set, std::unordered_map<std::size_t, typename string_type::traits_type::char_type>{});
		}

		template <typename string_type>
		string_type generate(std::size_t min_size, std::size_t max_size, char_set_type char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			set_range_list_type list;
			resolve_char_set<typename string_type::traits_type::char_type>(char_set, list);
			return generate<string_type>(min_size, max_size, list, delim);
		}

		template <typename string_type>
		string_type generate(std::size_t size, char_set_type char_set){
			return generate<string_type>(size, char_set, std::unordered_map<std::size_t, typename string_type::traits_type::char_type>{});
		}

		template <typename string_type>
		string_type generate(std::size_t size, char_set_type char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return generate<string_type>(size, size, char_set, delim);
		}

		template <typename char_type>
		static void resolve_char_set(char_set_type char_set, set_range_list_type &out){
			if (char_set == char_set_type::nil){//Include all
				out.push_back(set_range{ 0, static_cast<int>(std::numeric_limits<char_type>::max()) });
				return;
			}

			if (EWIN_IS(char_set, char_set_type::invert))
				char_set = ~char_set;//Exclude specified

			if (EWIN_IS(char_set, char_set_type::alpha)){//Alphabet range
				if (EWIN_IS_ANY(char_set, (char_set_type::uppercase | char_set_type::lowercase))){
					if (EWIN_IS(char_set, char_set_type::uppercase))//Uppercase range
						out.push_back(set_range{ 0x0041, 0x005A });

					if (EWIN_IS(char_set, char_set_type::lowercase))//Lowercase range
						out.push_back(set_range{ 0x0061, 0x007A });
				}
				else{//Full alphabet range
					out.push_back(set_range{ 0x0041, 0x005A });
					out.push_back(set_range{ 0x0061, 0x007A });
				}
			}

			if (EWIN_IS(char_set, char_set_type::digits)){//Digits range
				if (EWIN_IS(char_set, char_set_type::hex)){//Hexadecimal range
					out.push_back(set_range{ 0x0030, 0x0039 });
					if (EWIN_IS_ANY(char_set, (char_set_type::uppercase | char_set_type::lowercase))){
						if (EWIN_IS(char_set, char_set_type::uppercase))//Uppercase range
							out.push_back(set_range{ 0x0041, 0x0046 });

						if (EWIN_IS(char_set, char_set_type::lowercase))//Lowercase range
							out.push_back(set_range{ 0x0061, 0x0066 });
					}
					else{//Full hex range
						out.push_back(set_range{ 0x0041, 0x0046 });
						out.push_back(set_range{ 0x0061, 0x0066 });
					}
				}
				else if (EWIN_IS(char_set, char_set_type::oct))//Octal range
					out.push_back(set_range{ 0x0030, 0x0039 });
				else//Full digit range
					out.push_back(set_range{ 0x0030, 0x0037 });
			}
		}

	private:
		random_generator<distribution_type, engine_type, seeder_type> generator_;
	};

	using random_string = basic_random_string<>;

	template <class string_type, template <class> class distribution_type = std::uniform_int_distribution, class engine_type = std::mt19937, class seeder_type = std::random_device>
	class typed_random_string : public basic_random_string<distribution_type, engine_type, seeder_type>{
	public:
		typedef basic_random_string<distribution_type, engine_type, seeder_type> base_type;

		template <typename... args_types>
		explicit typed_random_string(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		string_type operator ()(std::size_t min_size, std::size_t max_size, const set_range_list_type &char_set){
			return base_type::generate<string_type>(min_size, max_size, char_set);
		}

		string_type operator ()(std::size_t min_size, std::size_t max_size, const set_range_list_type &char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return base_type::generate<string_type>(min_size, max_size, char_set, delim);
		}

		string_type operator ()(std::size_t size, const set_range_list_type &char_set){
			return base_type::generate<string_type>(size, char_set);
		}

		string_type operator ()(std::size_t size, const set_range_list_type &char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return base_type::generate<string_type>(size, char_set, delim);
		}

		string_type operator ()(std::size_t min_size, std::size_t max_size, char_set_type char_set){
			return base_type::generate<string_type>(min_size, max_size, char_set);
		}

		string_type operator ()(std::size_t min_size, std::size_t max_size, char_set_type char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return base_type::generate<string_type>(min_size, max_size, char_set, delim);
		}

		string_type operator ()(std::size_t size, char_set_type char_set){
			return base_type::generate<string_type>(size, char_set);
		}

		string_type operator ()(std::size_t size, char_set_type char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return base_type::generate<string_type>(size, char_set, delim);
		}
	};

	using random_narrow_string	= typed_random_string<std::string>;
	using random_wide_string	= typed_random_string<std::wstring>;
}

#endif /* !EWIN_RANDOM_STRING_H */
