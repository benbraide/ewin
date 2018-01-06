#pragma once

#ifndef EWIN_TIMER_H
#define EWIN_TIMER_H

#include <mutex>
#include <condition_variable>
#include <functional>

#include "raii.h"
#include "type_aliases.h"

#include "boolean_property.h"
#include "numeric_property.h"

namespace ewin::common{
	template <class app_type, class precision_type = std::nano>
	class timer{
	public:
		typedef app_type app_type;
		typedef precision_type precision_type;

		typedef std::chrono::duration<long long, precision_type> duration_type;
		typedef std::function<void(types::uint, types::uint, bool)> callback_type;

		typedef std::condition_variable condition_variable_type;
		typedef std::mutex lock_type;

		typedef std::lock_guard<lock_type> guard_type;
		typedef std::unique_lock<lock_type> unique_guard_type;

		struct cache_info{
			bool started;
			bool periodic;
			types::uint count;
			duration_type duration;
			duration_type delay;
			callback_type callback;
		};

		timer()
			: app_(nullptr){
			cache_ = cache_info{
				false,
				false
			};
			bind_properties_();
		}

		explicit timer(app_type &app)
			: app_(&app){
			cache_ = cache_info{
				false,
				false
			};
			bind_properties_();
		}

		~timer(){
			if (cache_.started)
				stop_();
		}

		boolean_value_property<timer> started;
		boolean_value_property<timer> periodic;

		value_property<duration_type, timer> duration;
		value_property<duration_type, timer> delay;

		numeric_value_property<types::uint, timer> count;
		value_property<callback_type, timer> callback;

	private:
		void bind_properties_(){
			started.initialize_(nullptr, [this](void *prop, void *arg, property_access access){
				if (access == property_access::write){
					if (*reinterpret_cast<bool *>(arg))
						start_();
					else//Stop
						stop_();
				}
				else//Read
					*reinterpret_cast<bool *>(arg) = cache_.started;
			});

			periodic.initialize_(&cache_.periodic, nullptr);

			duration.initialize_(&cache_.duration, nullptr);
			delay.initialize_(&cache_.delay, nullptr);

			count.initialize_(&cache_.count, nullptr);
			callback.initialize_(&cache_.callback, nullptr);
		}

		void start_(){
			if (cache_.started || cache_.callback == nullptr || cache_.duration == duration_type(0))
				return;

			raii_thread([this]{
				unique_guard_type lock(lock_);

				auto periodic = cache_.periodic;
				auto count = cache_.count;
				auto duration = cache_.duration;
				auto delay = cache_.delay;
				auto callback = cache_.callback;

				if (cache_.started || callback == nullptr || duration == duration_type(0))
					return;

				cache_.started = true;
				types::uint loop_count = 0;

				while (true){
					condition_.wait_for(lock, duration, [this]{
						return !cache_.started;
					});

					++loop_count;
					if (app_ == nullptr)
						raii_thread(callback, loop_count, count, cache_.started);
					else//Execute inside application's context
						app_->async_task += std::bind(callback, loop_count, count, cache_.started);

					if (!periodic || !cache_.started || (count > 0u && loop_count >= count))
						break;

					if (delay != duration_type(0)){
						condition_.wait_for(lock, delay, [this]{
							return cache_.started;
						});

						if (!cache_.started)
							break;
					}
				}

				cache_.started = false;
			});
		}

		void stop_(){
			guard_type guard(lock_);
			if (cache_.started){
				cache_.started = false;
				condition_.notify_one();
			}
		}

		app_type *app_;
		cache_info cache_;
		condition_variable_type condition_;
		lock_type lock_;
	};
}

#endif /* !EWIN_TIMER_H */
