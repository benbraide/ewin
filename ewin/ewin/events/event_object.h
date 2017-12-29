#pragma once

#ifndef EWIN_EVENT_OBJECT_H
#define EWIN_EVENT_OBJECT_H

#include <memory>
#include <bitset>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/object_property.h"

#include "../application/application_object.h"

namespace ewin::message{
	class target;
	class menu_target;
}

namespace ewin::window{
	class object;
}

namespace ewin::events{
	class callback;

	enum class object_state_type : unsigned int{
		nil						= (0 << 0x0000),
		default_called			= (1 << 0x0000),
		default_prevented		= (1 << 0x0001),
		propagation_stopped		= (1 << 0x0002),
		bubbled					= (1 << 0x0003),
	};

	EWIN_MAKE_OPERATORS(object_state_type);

	template <class target_type>
	class basic_object{
	public:
		typedef target_type target_type;
		typedef object_state_type state_type;

		typedef ewin::window::object window_type;

		basic_object(target_type *target, target_type *owner)
			: target_((target == nullptr) ? owner : target), owner_(owner), states_(state_type::nil){
			bind_properties_();
			if (target_ != owner_)
				EWIN_SET(states_, state_type::bubbled);
		}

		common::read_only_object_value_property<target_type, basic_object> target;
		common::read_only_object_value_property<target_type, basic_object> owner;

		common::boolean_value_property<basic_object> do_default;
		common::boolean_value_property<basic_object> prevent_default;
		common::boolean_value_property<basic_object> stop_propagation;

		common::read_only_boolean_value_property<basic_object> bubbled;
		common::read_only_boolean_value_property<basic_object> handled;

	protected:
		friend class ewin::message::target;

		void bind_properties_(){
			auto handler = EWIN_PROP_HANDLER(basic_object);

			target.initialize_(target_, nullptr);
			owner.initialize_(owner_, nullptr);

			do_default.initialize_(nullptr, handler);
			prevent_default.initialize_(nullptr, handler);
			stop_propagation.initialize_(nullptr, handler);

			bubbled.initialize_(nullptr, handler);
			handled.initialize_(nullptr, handler);
		}

		virtual void handle_property_(void *prop, void *arg, common::property_access access){
			if (prop == &prevent_default){
				if (access == common::property_access::read)
					*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::default_prevented);
				else if (access == common::property_access::write && *static_cast<bool *>(arg))
					prevent_default_();
			}
			else if (prop == &stop_propagation){
				if (access == common::property_access::read)
					*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::propagation_stopped);
				else if (access == common::property_access::write && *static_cast<bool *>(arg))
					EWIN_SET(states_, state_type::propagation_stopped);
			}
			else if (prop == &bubbled)
				*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::bubbled);
			else if (prop == &handled)
				*static_cast<bool *>(arg) = EWIN_IS_ANY(states_, state_type::default_called | state_type::default_prevented | state_type::bubbled);
		}

		virtual void do_default_(){
			if (!EWIN_IS_ANY(states_, state_type::default_called | state_type::default_prevented | state_type::bubbled))
				EWIN_SET(states_, state_type::default_called);
		}

		virtual void prevent_default_(){
			if (!EWIN_IS_ANY(states_, state_type::default_called | state_type::default_prevented | state_type::bubbled))
				EWIN_SET(states_, state_type::default_prevented);
		}

		target_type *target_;
		target_type *owner_;
		state_type states_;
	};

	using object = basic_object<ewin::message::target>;
	using menu_object = basic_object<ewin::message::menu_target>;

	class message : public object{
	public:
		typedef callback callback_type;

		template <typename... args_types>
		message(common::types::msg &msg, callback_type &callback, args_types &&... args)
			: object(std::forward<args_types>(args)...), msg_(&msg), callback_(callback), result_(0){
			info.initialize_(msg_, nullptr);
			result.initialize_(nullptr, EWIN_PROP_HANDLER(message));
		}

		virtual ~message();

		common::read_only_object_value_property<common::types::msg, message> info;
		common::value_property<common::types::result, message> result;

	protected:
		friend class ewin::message::target;

		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual void do_default_() override;

		virtual common::types::result call_default_();

		virtual void bubble_();

		virtual void remove_bubble_();

		common::types::msg *msg_;
		callback_type &callback_;
		common::types::result result_;
	};

	class mouse_activate : public message{
	public:
		template <typename... args_types>
		explicit mouse_activate(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(mouse_activate);

			hit_target.initialize_(nullptr, handler);
			mouse_message.initialize_(nullptr, handler);
		}

		common::read_only_value_property<common::types::uint, mouse_activate> hit_target;
		common::read_only_value_property<common::types::uint, mouse_activate> mouse_message;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	using cursor = mouse_activate;

	class pre_activate : public message{
	public:
		template <typename... args_types>
		explicit pre_activate(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			is_activating.initialize_(nullptr, EWIN_PROP_HANDLER(pre_activate));
		}

		common::read_only_boolean_value_property<pre_activate> is_activating;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	class activate : public message{
	public:
		template <typename... args_types>
		explicit activate(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(activate);

			is_activated.initialize_(nullptr, handler);
			is_click_activated.initialize_(nullptr, handler);
		}

		common::read_only_boolean_value_property<activate> is_activated;
		common::read_only_boolean_value_property<activate> is_click_activated;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	template <common::types::uint changing_message>
	class change : public message{
	public:
		template <typename... args_types>
		explicit change(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			is_changing.initialize_(nullptr, EWIN_PROP_HANDLER(change));
		}

		common::read_only_boolean_value_property<change> is_changing;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override{
			if (prop == &is_changing)
				*static_cast<bool *>(arg) = (msg_->message == changing_message);
			else//Forward
				message::handle_property_(prop, arg, access);
		}
	};

	using focus_change		= change<WM_SETFOCUS>;
	using position_change	= change<WM_WINDOWPOSCHANGING>;
	using size				= change<WM_SIZING>;
	using move				= change<WM_MOVING>;

	class enable : public message{
	public:
		template <typename... args_types>
		explicit enable(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			is_enabled.initialize_(nullptr, EWIN_PROP_HANDLER(enable));
		}

		common::read_only_boolean_value_property<enable> is_enabled;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	class hit_test : public message{
	public:
		template <typename... args_types>
		explicit hit_test(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			mouse_position.initialize_(nullptr, EWIN_PROP_HANDLER(hit_test));
		}

		common::read_only_value_property<common::types::point, hit_test> mouse_position;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	class style_change : public change<WM_STYLECHANGING>{
	public:
		typedef change<WM_STYLECHANGING> base_type;

		template <typename... args_types>
		explicit style_change(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(style_change);

			is_extended.initialize_(nullptr, handler);
			styles.initialize_(nullptr, handler);
		}

		virtual ~style_change();

		common::read_only_boolean_value_property<style_change> is_extended;
		common::read_only_value_property<common::types::style_struct *, style_change> styles;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	class draw : public message{
	public:
		typedef std::function<void()> cleanup_callback_type;

		template <typename... args_types>
		explicit draw(args_types &&... args)
			: message(std::forward<args_types>(args)...), drawer_(nullptr), color_brush_(nullptr){
			auto handler = EWIN_PROP_HANDLER(draw);

			drawer.initialize_(nullptr, handler);
			color_brush.initialize_(nullptr, handler);

			clip.initialize_(nullptr, handler);
			erase_background.initialize_(nullptr, handler);
			paint_info.initialize_(nullptr, handler);

			info_.fErase = info_.fIncUpdate = info_.fRestore = FALSE;
			info_.hdc = nullptr;
			info_.rcPaint = common::types::rect{};
		}

		virtual ~draw();

		common::read_only_object_value_property<drawing::object, draw> drawer;
		common::read_only_object_value_property<drawing::solid_color_brush, draw> color_brush;

		common::read_only_rect_value_property<common::types::rect, draw> clip;
		common::read_only_boolean_value_property<draw> erase_background;
		common::read_only_object_value_property<common::types::paint_struct, draw> paint_info;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual void begin_paint_();

		virtual void bind_(drawing::hdc_object &drawer);

		common::types::paint_struct info_;
		drawing::object *drawer_;
		drawing::solid_color_brush *color_brush_;
		cleanup_callback_type cleanup_;
	};

	class mouse : public message{
	public:
		struct cache{
			common::types::uint hit;
			common::types::point position;
			common::types::size delta;
		};

		template <typename... args_types>
		explicit mouse(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			cache_values_();
			hit.initialize_(&cache_.hit, nullptr);
			position.initialize_(&cache_.position, nullptr);
			delta.initialize_(&cache_.delta, nullptr);
		}

		virtual ~mouse();

		common::read_only_value_property<common::types::uint, mouse> hit;
		common::read_only_point_value_property<common::types::point, mouse> position;
		common::read_only_size_value_property<common::types::size, mouse> delta;

	protected:
		void cache_values_();

		cache cache_;
	};

	class mouse_drag : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_drag(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){
			button_ = static_cast<common::types::uint>(msg_->wParam);
			button.initialize_(&button_, nullptr);
		}

		virtual ~mouse_drag();

		common::read_only_value_property<common::types::uint, mouse_drag> button;

	protected:
		common::types::uint button_;
	};

	class context_menu : public message{
	public:
		template <typename... args_types>
		explicit context_menu(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			cache_values_();
			position.initialize_(&position_, nullptr);
		}

		virtual ~context_menu();

		common::read_only_point_value_property<common::types::point, context_menu> position;

	protected:
		void cache_values_();

		common::types::point position_;
	};

	class key : public message{
	public:
		typedef std::unordered_map<common::types::byte, bool> key_state_map_type;

		struct cache{
			unsigned short code;
			char scan_code;
			bool extended;
		};

		template <typename... args_types>
		explicit key(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			cache_values_();
			code.initialize_(&cache_.code, nullptr);
			scan_code.initialize_(&cache_.scan_code, nullptr);
			extended.initialize_(&cache_.extended, nullptr);
			state.initialize_(nullptr, [](void *prop, void *arg, common::property_access access){
				*static_cast<common::types::uint *>(arg) = retrieve_key_states();
			});
		}

		virtual ~key();

		common::read_only_value_property<unsigned short, key> code;
		common::read_only_value_property<char, key> scan_code;
		common::read_only_boolean_value_property<key> extended;
		common::read_only_value_property<common::types::uint, key> state;

		static common::types::uint retrieve_key_states();

		static thread_local common::types::byte keyboard_states[0x100];

	protected:
		void cache_values_();

		cache cache_;
	};

	class key_down : public key{
	public:
		struct extended_cache{
			bool first;
			common::types::word repeat_count;
		};

		template <typename... args_types>
		explicit key_down(args_types &&... args)
			: key(std::forward<args_types>(args)...){
			cache_values_();
			first.initialize_(&extended_cache_.first, nullptr);
			repeat_count.initialize_(&extended_cache_.repeat_count, nullptr);
		}

		virtual ~key_down();

		common::read_only_boolean_value_property<key_down> first;
		common::read_only_value_property<common::types::word, key_down> repeat_count;

	protected:
		void cache_values_();

		extended_cache extended_cache_;
	};

	class key_press : public key_down{
	public:
		template <typename... args_types>
		explicit key_press(args_types &&... args)
			: key_down(std::forward<args_types>(args)...){
			cache_values_();
			released.initialize_(&released_, nullptr);
		}

		virtual ~key_press();

		common::read_only_boolean_value_property<key_press> released;

	protected:
		void cache_values_();

		bool released_;
	};
}

#endif /* !EWIN_EVENT_OBJECT_H */
