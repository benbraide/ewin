#pragma once

#ifndef EWIN_WINDOW_TRANSITION_H
#define EWIN_WINDOW_TRANSITION_H

#include "../common/type_aliases.h"

#include "../common/point_property.h"
#include "../common/size_property.h"
#include "../common/rect_property.h"

#include "../animation/animation_transition.h"

namespace ewin::window{
	class object;

	class wnd_transition{
	public:
		struct options{
			ewin::animation::easing_type type;
			ewin::animation::easing_mode mode;
			float duration;
			int rate;
		};

		struct ids{
			std::size_t position;
			std::size_t size;
			std::size_t color;
		};

		template <class prop_type>
		struct serialize_prop;

		template <>
		struct serialize_prop<common::point_value_property<common::types::point, object>>{
			template <typename app_type>
			static void serialize(common::point_value_property<common::types::point, object> &prop, const common::types::point &end,
				std::vector<typename animation::transition<app_type>::limit_info> &list){
				list.reserve(2);
				list.push_back(typename animation::transition<app_type>::limit_info{ static_cast<float>(prop.x), static_cast<float>(end.x) });
				list.push_back(typename animation::transition<app_type>::limit_info{ static_cast<float>(prop.y), static_cast<float>(end.y) });
			}
		};

		template <>
		struct serialize_prop<common::size_value_property<common::types::size, object>>{
			template <typename app_type>
			static void serialize(common::size_value_property<common::types::size, object> &prop, const common::types::size &end,
				std::vector<typename animation::transition<app_type>::limit_info> &list){
				list.reserve(2);
				list.push_back(typename animation::transition<app_type>::limit_info{ static_cast<float>(prop.width), static_cast<float>(end.cx) });
				list.push_back(typename animation::transition<app_type>::limit_info{ static_cast<float>(prop.height), static_cast<float>(end.cy) });
			}
		};

		template <class prop_type>
		struct deserialize_prop;

		template <>
		struct deserialize_prop<common::point_value_property<common::types::point, object>>{
			static void deserialize(common::point_value_property<common::types::point, object> &prop, const std::vector<float> &list){
				prop = common::types::point{
					static_cast<int>(list[0]),
					static_cast<int>(list[1])
				};
			}
		};

		template <>
		struct deserialize_prop<common::size_value_property<common::types::size, object>>{
			static void deserialize(common::size_value_property<common::types::size, object> &prop, const std::vector<float> &list){
				prop = common::types::size{
					static_cast<int>(list[0]),
					static_cast<int>(list[1])
				};
			}
		};

		explicit wnd_transition(object &owner);

		virtual ~wnd_transition();

		common::size_value_property<common::types::size, wnd_transition> size;
		common::size_value_property<common::types::size, wnd_transition> client_size;

		common::point_value_property<common::types::point, wnd_transition> position;
		common::point_value_property<common::types::point, wnd_transition> relative_position;

	protected:
		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		template <typename app_type, typename target_type, typename value_type>
		void single_(std::size_t *id, app_type *app, target_type *target, value_type value){
			if (*id != 0u)//Cancel previous
				app->transition -= *id;

			typename animation::transition<app_type>::single_info_type info{
				options_.type,
				options_.mode,
				static_cast<float>(*target),
				static_cast<float>(value),
				options_.duration,
				options_.rate
			};

			info.callback = [this, id, app, target](float value, bool done){
				if (static_cast<int>(value) != static_cast<int>(*target))
					*target = value;//Update

				if (done)//Reset ID
					*id = 0;
			};

			*id = (app->transition += info);
		}

		template <typename app_type, typename target_type>
		void multiple_(std::size_t *id, app_type *app, target_type *target, const typename target_type::value_type &value){
			if (*id != 0u)//Cancel previous
				app->transition -= *id;

			std::vector<typename animation::transition<app_type>::limit_info> limits;
			serialize_prop<target_type>::serialize<app_type>(*target, value, limits);

			typename animation::transition<app_type>::multiple_info_type info{
				options_.type,
				options_.mode,
				limits,
				options_.duration,
				options_.rate
			};

			info.callback = [this, id, app, target](const std::vector<float> &values, bool done){
				deserialize_prop<target_type>::deserialize(*target, values);
				if (done)//Reset ID
					*id = 0;
			};

			*id = (app->transition += info);
		}

		object &owner_;
		options options_;
		ids ids_;
	};
}

#endif /* !EWIN_WINDOW_TRANSITION_H */
