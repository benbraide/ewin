#pragma once

#ifndef EWIN_BASIC_EVENT_H
#define EWIN_BASIC_EVENT_H

#include <variant>
#include <functional>
#include <unordered_map>

#include "../application/application_object.h"

#include "event_object.h"

namespace ewin::events{
	template <class target_type>
	class generic_basic{
	public:
		typedef target_type base_target_type;

		virtual ~generic_basic() = default;

	protected:
		friend target_type;

		virtual void fire_(basic_message<base_target_type> &e) = 0;
	};

	template <class object_type, class target_type>
	class typed_basic : public generic_basic<typename message_target_from_object<target_type>::type>{
	private:
		struct dummy_type{};

		struct dummy_type_other{};

	public:
		typedef generic_basic<typename message_target_from_object<target_type>::type> base_type;
		typedef typename base_type::base_target_type base_target_type;

		typedef object_type object_type;
		typedef target_type target_type;

		typedef std::function<void()> no_arg_callback_type;
		typedef std::function<void(object_type &)> callback_type;

		typedef std::conditional_t<
			std::is_same_v<object_type, basic_message<base_target_type>>,
			dummy_type,
			std::function<void(basic_message<base_target_type> &)>
		> base_callback_type;

		typedef std::conditional_t<
			std::is_same_v<object_type, basic_object<base_target_type>>,
			dummy_type_other,
			std::function<void(basic_object<base_target_type> &)>
		> low_level_callback_type;

		typedef std::variant<callback_type, base_callback_type, low_level_callback_type, no_arg_callback_type> generic_callback_type;
		typedef std::unordered_map<std::size_t, generic_callback_type> map_type;

		struct callback_visitor{
			explicit callback_visitor(object_type &e)
				: e_(&e){}

			void operator()(const callback_type &callback) const{
				callback(*e_);
			}

			template <typename unused_type = object_type>
			std::enable_if_t<!std::is_same_v<unused_type, basic_message<base_target_type>> &&
				!std::is_same_v<unused_type, basic_object<base_target_type>>, void> operator()(const base_callback_type &callback) const{
				callback(*e_);
			}

			template <typename unused_type = object_type>
			std::enable_if_t<!std::is_same_v<unused_type, basic_object<base_target_type>>, void> operator()(const low_level_callback_type &callback) const{
				callback(*e_);
			}

			void operator()(const no_arg_callback_type &callback) const{
				callback();
			}

			void operator()(const dummy_type &dummy) const{}

			void operator()(const dummy_type_other &dummy) const{}

			object_type *e_;
		};

		explicit typed_basic(target_type &target)
			: target_(&target){}

		std::size_t operator +=(const callback_type &callback){
			return add_(callback);
		}

		template <typename unused_type = object_type>
		std::enable_if_t<!std::is_same_v<unused_type, basic_message<base_target_type>> &&
			!std::is_same_v<unused_type, basic_object<base_target_type>>, std::size_t> operator +=(const base_callback_type &callback){
			return add_(callback);
		}

		template <typename unused_type = object_type>
		std::enable_if_t<!std::is_same_v<unused_type, basic_object<base_target_type>>, std::size_t> operator +=(const low_level_callback_type &callback){
			return add_(callback);
		}

		std::size_t operator +=(const no_arg_callback_type &callback){
			return add_(callback);
		}

		void operator -=(std::size_t id){
			if (target_->created && target_->app_ != nullptr){
				target_->app_->task += [&]{//Execute in target's thread
					remove_(id);
				};
			}
			else//Execute in current context
				remove_(id);
		}

	private:
		friend base_target_type;

		template <typename function_type>
		std::size_t add_(const function_type &callback){
			std::size_t id = 0u;
			if (target_->created && target_->app_ != nullptr){//Execute in target's thread
				target_->app_->task += [&]{
					map_[id = target_->app_->integer_generator(static_cast<std::size_t>(1), std::numeric_limits<std::size_t>::max())] = callback;
					target_->event_listener_count_changed_(*this, map_.size());
				};
			}
			else{//Execute in current context
				map_[id = application::object::integer_generator(static_cast<std::size_t>(1), std::numeric_limits<std::size_t>::max())] = callback;
				target_->event_listener_count_changed_(*this, map_.size());
			}

			return id;
		}

		void remove_(std::size_t id){
			auto entry = map_.find(id);
			if (entry != map_.end()){
				map_.erase(entry);
				target_->event_listener_count_changed_(*this, map_.size());
			}
		}

		virtual void fire_(basic_message<base_target_type> &e) override{
			callback_visitor visitor(*dynamic_cast<object_type *>(&e));
			for (auto &entry : map_)//Call target
				std::visit(visitor, entry.second);
		}

		target_type *target_;
		map_type map_;
	};

	using basic = generic_basic<ewin::message::target>;
	using menu_basic = generic_basic<ewin::message::menu_target>;
}

#endif /* !EWIN_BASIC_EVENT_H */
