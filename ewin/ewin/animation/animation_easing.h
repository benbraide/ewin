#pragma once

#ifndef EWIN_ANIMATION_EASING_H
#define EWIN_ANIMATION_EASING_H

#include <cmath>

#define EWIN_PI  3.14159265f

namespace ewin::animation{
	enum class easing_type{
		linear,
		back,
		bounce,
		circle,
		cubic,
		elastic,
		exponential,
		quadratic,
		quart,
		quint,
		sine,
	};

	enum class easing_mode{
		in,
		out,
		inout,
	};

	template <easing_type type, easing_mode mode>
	struct easing{
		static constexpr float ease(float time, float change, float duration){
			return (change * (time / duration));
		}
	};

	template <>
	struct easing<easing_type::back, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			time /= duration;
			return (change * time * time * ((time * 2.70158f) - 1.70158f));
		}
	};

	template <>
	struct easing<easing_type::back, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			time = (time / duration) - 1;
			return (change * (time * time * ((time * 2.70158f) + 1.70158f) + 1.0f));
		}
	};

	template <>
	struct easing<easing_type::back, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			time /= (duration / 2);

			auto constant = (1.70158f * 1.525f);
			if (time < 1.0f)
				return ((change / 2.0f) * (time * time * ((constant + 1.0f) * time - constant)));

			time -= 2.0f;
			return ((change / 2.0f) * (time * time * ((constant + 1.0f) * time + constant) + 2.0f));
		}
	};

	template <>
	struct easing<easing_type::bounce, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			time /= duration;
			if (time < (1.0f / 2.75f))
				return (change * 7.5625f * time * time);

			if (time < (2.0f / 2.75f)){
				time -= (1.5f / 2.75f);
				return (change * ((7.5625f * time * time) + 0.75f));
			}

			if (time < (2.5f / 2.75f)){
				time -= (2.25f / 2.75f);
				return (change * ((7.5625f * time * time) + 0.9375f));
			}

			time -= (2.625f / 2.75f);
			return (change * ((7.5625f * time * time) + 0.984375f));
		}
	};

	template <>
	struct easing<easing_type::bounce, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			return (change - easing<easing_type::bounce, easing_mode::out>::ease(duration - time, change, duration));
		}
	};

	template <>
	struct easing<easing_type::bounce, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			if (time < (duration / 2.0f))
				return (easing<easing_type::bounce, easing_mode::in>::ease((time * 2.0f), change, duration) * 0.5f);
			return ((easing<easing_type::bounce, easing_mode::out>::ease((time * 2.0f - duration), change, duration) * 0.5f) + (change * 0.5f));
		}
	};

	template <>
	struct easing<easing_type::circle, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			time /= duration;
			return ((-change * (std::sqrt(1.0f - (time * time))) - 1.0f));
		}
	};

	template <>
	struct easing<easing_type::circle, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			time = ((time / duration) - 1.0f);
			return (change * std::sqrt(1.0f - (time * time)));
		}
	};

	template <>
	struct easing<easing_type::circle, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			time /= (duration / 2.0f);
			if (time < 1.0f)
				return (((-change / 2.0f) * (std::sqrt(1.0f - time * time)) - 1.0f));
			return (((change / 2.0f) * (std::sqrt(1.0f - time * (time - 2.0f))) + 1.0f));
		}
	};

	template <>
	struct easing<easing_type::cubic, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			time /= duration;
			return (change * time * time * time);
		}
	};

	template <>
	struct easing<easing_type::cubic, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			time = ((time / duration) - 1.0f);
			return (change * ((time * time * time) + 1.0f));
		}
	};

	template <>
	struct easing<easing_type::cubic, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			time /= (duration / 2.0f);
			if (time < 1.0f)
				return ((change / 2.0f) * time * time * time);

			time -= 2.0f;
			return ((change / 2.0f) * ((time * time * time) + 2.0f));
		}
	};

	template <>
	struct easing<easing_type::elastic, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			if (time == 0.0f)
				return 0.0f;

			time /= duration;
			if (time == 1.0f)
				return change;

			auto p = (duration * 0.3f);
			auto s = (p / 4.0f);

			time -= 1.0f;
			return -(change * std::pow(2.0f, (10.0f * time)) * std::sin(((time * duration) - s) * ((2.0f * EWIN_PI) / p)));
		}
	};

	template <>
	struct easing<easing_type::elastic, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			if (time == 0.0f)
				return 0.0f;

			time /= duration;
			if (time == 1.0f)
				return change;

			auto p = (duration * 0.3f);
			auto s = (p / 4.0f);

			return ((change * std::pow(2.0f, (-10.0f * time)) * std::sin((((time * duration) - s) * 2.0f * EWIN_PI) / p)) + change);
		}
	};

	template <>
	struct easing<easing_type::elastic, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			if (time == 0.0f)
				return 0.0f;

			time /= (duration / 2.0f);
			if (time == 2.0f)
				return change;

			auto p = (duration * (0.3f * 1.5f));
			auto s = (p / 4.0f);

			if (time < 1.0f){
				time -= 1.0f;
				return (-0.5f * change * std::pow(2.0f, 10.0f * time) * std::sin(((time * duration - s) * 2.0f * EWIN_PI) / p));
			}

			time -= 1.0f;
			return ((change * std::pow(2.0f, -10.0f * time) * std::sin(((time * duration - s) * 2.0f * EWIN_PI) / p) * 0.5f) + change);
		}
	};

	template <>
	struct easing<easing_type::exponential, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			return ((time == 0.0f) ? 0.0f : (change * std::pow(2.0f, 10.0f * ((time / duration) - 1.0f))));
		}
	};

	template <>
	struct easing<easing_type::exponential, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			return ((time == duration) ? change : (change * (-std::pow(2.0f, ((-10.0f * time) / duration)) + 1.0f)));
		}
	};

	template <>
	struct easing<easing_type::exponential, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			if (time == 0.0f)
				return 0.0f;

			if (time == duration)
				return change;

			time /= (duration / 2);
			if (time < 1.0f)
				return ((change / 2.0f) * std::pow(2.0f, (10.0f * (time - 1.0f))));

			return ((change / 2.0f) * (-std::pow(2.0f, (-10.0f * (time - 1.0f))) + 2.0f));
		}
	};

	template <>
	struct easing<easing_type::quadratic, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			time /= duration;
			return (change * time * time);
		}
	};

	template <>
	struct easing<easing_type::quadratic, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			time /= duration;
			return (-change * time * (time - 2.0f));
		}
	};

	template <>
	struct easing<easing_type::quadratic, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			time /= (duration / 2.0f);
			if (time < 1.0f)
				return ((change / 2.0f) * time * time);
			return (-change / 2.0f * (((time - 2.0f) * (time - 1.0f)) - 1.0f));
		}
	};

	template <>
	struct easing<easing_type::quart, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			time /= duration;
			return (change * time * time * time * time);
		}
	};

	template <>
	struct easing<easing_type::quart, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			time = ((time / duration) - 1.0f);
			return (-change * ((time * time * time * time) - 1.0f));
		}
	};

	template <>
	struct easing<easing_type::quart, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			time /= (duration / 2.0f);
			if (time < 1.0f)
				return ((change / 2.0f) * time * time * time * time);

			time -= 2.0f;
			return ((-change / 2.0f) * ((time * time * time * time) - 2.0f));
		}
	};

	template <>
	struct easing<easing_type::quint, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			time /= duration;
			return (change * time * time * time * time * time);
		}
	};

	template <>
	struct easing<easing_type::quint, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			time = ((time / duration) - 1.0f);
			return (change * ((time * time * time * time * time) + 1.0f));
		}
	};

	template <>
	struct easing<easing_type::quint, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			time /= (duration / 2.0f);
			if (time < 1.0f)
				return ((change / 2.0f) * time * time * time * time * time);

			time -= 2.0f;
			return ((change / 2.0f) * ((time * time * time * time * time) + 2.0f));
		}
	};

	template <>
	struct easing<easing_type::sine, easing_mode::in>{
		static constexpr float ease(float time, float change, float duration){
			return ((-change * std::cos((time / duration) * (EWIN_PI / 2.0f))) + change);
		}
	};

	template <>
	struct easing<easing_type::sine, easing_mode::out>{
		static constexpr float ease(float time, float change, float duration){
			return (change * std::sin((time / duration) * (EWIN_PI / 2.0f)));
		}
	};

	template <>
	struct easing<easing_type::sine, easing_mode::inout>{
		static constexpr float ease(float time, float change, float duration){
			return ((-change / 2.0f) * (std::cos((EWIN_PI * (time / duration))) - 1.0f));
		}
	};
}

#endif /* !EWIN_ANIMATION_EASING_H */
