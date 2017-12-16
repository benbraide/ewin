#pragma once

#ifndef EWIN_BASIC_EVENT_H
#define EWIN_BASIC_EVENT_H

#include <variant>
#include <functional>
#include <unordered_map>

#include "event_object.h"

namespace ewin::events{
	class basic{
	public:
		virtual ~basic() = default;

	protected:
		friend class ewin::message::target;
		template <class, class, bool> friend class typed_basic;

		virtual void fire_(message &e) = 0;
	};

	template <class object_type, class target_type, bool can_be_propagated>
	class typed_basic : public basic{
	public:
		typedef object_type object_type;
		typedef target_type target_type;

		typedef std::function<void(object_type &)> callback_type;
		typedef std::function<void()> no_arg_callback_type;

		typedef void(*raw_callback_type)(object_type &);
		typedef void(*no_arg_raw_callback_type)();

		typedef std::variant<raw_callback_type, no_arg_raw_callback_type, callback_type, no_arg_callback_type> generic_callback_type;
		typedef std::unordered_map<std::size_t, generic_callback_type> map_type;

		struct callback_visitor{
			explicit callback_visitor(object_type &e)
				: e_(&e){}

			void operator()(const callback_type &callback) const{
				callback(*e_);
			}

			void operator()(const no_arg_callback_type &callback) const{
				callback();
			}

			void operator()(const raw_callback_type &callback) const{
				callback(*e_);
			}

			void operator()(const no_arg_raw_callback_type &callback) const{
				callback();
			}

			object_type *e_;
		};

		explicit typed_basic(target_type &target)
			: target_(&target){}

		template <typename function_type>
		std::size_t operator +=(function_type callback){
			std::size_t id = 0u;
			target_->app->task += [&]{//Execute in target's thread
				map_[id] = callback;
			};

			return id;
		}

		void operator -=(std::size_t id){
			target_->app->task += [&]{//Execute in target's thread
				auto entry = map_.find(id);
				if (entry != map_.end())
					map_.erase(entry);
			};
		}

	private:
		friend class ewin::message::target;

		virtual void fire_(message &e) override{
			callback_visitor visitor(*dynamic_cast<object_type *>(&e));
			for (auto &entry : map_)//Call target
				std::visit(visitor, entry.second);

			if (!can_be_propagated || e.stop_propagation)
				return;

			auto bubbled = target_->bubble_event[*this];
			if (bubbled != nullptr){//Bubble event
				e.bubble = true;
				bubbled->fire_(e);
				e.remove_bubble_();
			}
		}

		target_type *target_;
		map_type map_;
	};
}

#endif /* !EWIN_BASIC_EVENT_H */
