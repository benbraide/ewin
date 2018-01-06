#pragma once

#ifndef EWIN_ANIMATION_TRANSITION_H
#define EWIN_ANIMATION_TRANSITION_H

#include <vector>
#include <functional>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <memory>

#include "../common/timer.h"

#include "animation_easing.h"

namespace ewin::animation{
	template <class app_type>
	class transition{
	public:
		typedef app_type app_type;
		typedef common::timer<app_type, std::milli> timer_type;

		struct limit_info{
			float start;
			float end;
		};

		typedef std::vector<limit_info> list_type;
		typedef std::function<void(float)> callback_type;
		typedef std::function<void(const std::vector<float> &)> multiple_callback_type;

		typedef std::shared_ptr<timer_type> timer_ptr_type;
		typedef std::unordered_map<std::size_t, timer_ptr_type> timer_list_type;

		struct single_info_type{
			easing_type type;
			easing_mode mode;
			float start;
			float end;
			float duration;
			int rate;
			callback_type callback;
		};

		struct multiple_info_type{
			easing_type type;
			easing_mode mode;
			list_type limits;
			float duration;
			int rate;
			multiple_callback_type callback;
		};

		explicit transition(app_type &app)
			: app_(app){}

		std::size_t operator +=(const single_info_type &info){
			if (info.start == info.end || info.duration == 0.0f || info.rate == 0 || info.callback == nullptr)
				return 0;

			std::size_t id = 0;
			app_.task += [&]{
				id = app_.integer_generator(static_cast<std::size_t>(1), std::numeric_limits<std::size_t>::max());
				auto timer = std::make_shared<timer_type>(app_);

				auto change = (info.end - info.start);
				{//Set properties
					timer->periodic = true;
					timer->count = static_cast<common::types::uint>((info.rate * info.duration) + 0.5f);
					timer->duration = std::chrono::milliseconds(1000 / info.rate);
					timer->callback = [this, id, change, info](common::types::uint frame, common::types::uint frames, bool active){
						if (!active || frame >= frames){//Done transition
							if (active)
								info.callback(info.end);
							operator -=(id);//Remove
						}
						else//Continue
							info.callback(info.start + ease(info.type, info.mode, static_cast<float>(frame), change, static_cast<float>(frames)));
					};
				}

				(timer_list_[id] = timer)->started = true;
			};

			return id;
		}

		std::size_t operator +=(const multiple_info_type &info){
			if (info.limits.empty() || info.duration == 0.0f || info.rate == 0 || info.callback == nullptr)
				return 0;

			std::size_t id = 0;
			app_.task += [&]{
				id = app_.integer_generator(static_cast<std::size_t>(1), std::numeric_limits<std::size_t>::max());
				auto timer = std::make_shared<timer_type>(app_);

				std::vector<float> changes;
				changes.reserve(info.limits.size());
				for (auto &limit : info.limits)//Compute ranges
					changes.push_back(limit.end - limit.start);

				{//Set properties
					timer->periodic = true;
					timer->count = static_cast<common::types::uint>((info.rate * info.duration) + 0.5f);
					timer->duration = std::chrono::milliseconds(1000 / info.rate);
					timer->callback = [this, id, changes, info](common::types::uint frame, common::types::uint frames, bool active){
						static std::vector<float> offsets;
						if (offsets.empty())//Reset lists
							offsets.resize(changes.size(), 0.0f);

						if (!active || frame >= frames){//Done transition
							if (active){//Last call
								auto offsets_end = offsets.end();
								for (auto limit = info.limits.begin(), offset = offsets.begin(); offset != offsets_end; ++limit, ++offset)
									*offset = limit->end;
								info.callback(offsets);
							}

							offsets.clear();
							operator -=(id);//Remove
						}
						else{//Continue
							auto changes_end = changes.end();
							for (auto change = changes.begin(), offset = offsets.begin(); change != changes_end; ++change, ++offset){
								auto value = ease(info.type, info.mode, static_cast<float>(frame), *change, static_cast<float>(frames));
								*offset = (info.start + value);
							}

							info.callback(offsets);
						}
					};
				}

				(timer_list_[id] = timer)->started = true;
			};

			return id;
		}

		void operator -=(std::size_t id){
			app_.task += [&]{
				if (timer_list_.empty())
					return;

				auto entry = timer_list_.find(id);
				if (entry != timer_list_.end())
					timer_list_.erase(entry);
			};
		}

		static float ease(easing_type type, easing_mode mode, float time, float change, float duration){
			switch (type){
			case easing_type::back:
				return ease<easing_type::back>(mode, time, change, duration);
			case easing_type::bounce:
				return ease<easing_type::bounce>(mode, time, change, duration);
			case easing_type::circle:
				return ease<easing_type::circle>(mode, time, change, duration);
			case easing_type::cubic:
				return ease<easing_type::cubic>(mode, time, change, duration);
			case easing_type::elastic:
				return ease<easing_type::elastic>(mode, time, change, duration);
			case easing_type::exponential:
				return ease<easing_type::exponential>(mode, time, change, duration);
			case easing_type::quadratic:
				return ease<easing_type::quadratic>(mode, time, change, duration);
			case easing_type::quart:
				return ease<easing_type::quart>(mode, time, change, duration);
			case easing_type::quint:
				return ease<easing_type::quint>(mode, time, change, duration);
			case easing_type::sine:
				return ease<easing_type::sine>(mode, time, change, duration);
			default:
				break;
			}

			return ease<easing_type::linear>(mode, time, change, duration);
		}

		template <easing_type type>
		static float ease(easing_mode mode, float time, float change, float duration){
			switch (mode){
			case easing_mode::out:
				return ease<type, easing_mode::out>(time, change, duration);
			case easing_mode::inout:
				return ease<type, easing_mode::inout>(time, change, duration);
			default:
				break;
			}

			return ease<type, easing_mode::in>(time, change, duration);
		}

		template <easing_type type, easing_mode mode>
		static float ease(float time, float change, float duration){
			return easing<type, mode>::ease(time, change, duration);
		}

	private:
		app_type &app_;
		timer_list_type timer_list_;
	};
}

#endif /* !EWIN_ANIMATION_TRANSITION_H */
