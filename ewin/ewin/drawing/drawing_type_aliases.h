#pragma once

#ifndef EWIN_DRAWING_TYPE_ALIASES_H
#define EWIN_DRAWING_TYPE_ALIASES_H

#include "../common/common_headers.h"

namespace ewin::drawing{
	namespace d2d1 = ::D2D1;
}

namespace ewin::drawing::types{
	typedef ::HRESULT							result;
	typedef ::D2D1_COLOR_F						color;

	typedef ::D2D1_POINT_2F						point;
	typedef ::D2D1_SIZE_F						size;
	typedef ::D2D1_SIZE_U						usize;

	typedef ::D2D1_MATRIX_3X2_F					matrix_3x2;
	typedef ::D2D1_MATRIX_4X3_F					matrix_4x3;
	typedef ::D2D1_MATRIX_4X4_F					matrix_4x4;
	typedef ::D2D1_MATRIX_5X4_F					matrix_5x4;

	typedef ::DXGI_FORMAT						dxgi_format;
	typedef ::D2D1_ALPHA_MODE					alpha_mode;

	typedef ::D2D1_RENDER_TARGET_USAGE			render_target_usage;
	typedef ::D2D1_FEATURE_LEVEL				feature_level;
	typedef ::D2D1_PRESENT_OPTIONS				present_options;

	typedef ::D2D1_PIXEL_FORMAT					pixel_format;
	typedef ::D2D1_RENDER_TARGET_PROPERTIES		render_target_properties;

	typedef ::D2D1_ANTIALIAS_MODE				anti_alias_mode;
	typedef ::D2D1_TEXT_ANTIALIAS_MODE			text_anti_alias_mode;

	typedef ::ID2D1Brush						brush;
	typedef ::ID2D1BitmapBrush					bitmap_brush;
	typedef ::ID2D1SolidColorBrush				solid_color_brush;
	typedef ::ID2D1LinearGradientBrush			linear_gradient_brush;
	typedef ::ID2D1RadialGradientBrush			radial_gradient_brush;

	typedef ::ID2D1RenderTarget					render_target;
	typedef ::ID2D1BitmapRenderTarget			bitmap_render_target;
	typedef ::ID2D1HwndRenderTarget				hwnd_render_target;
	typedef ::D2D1_RENDER_TARGET_TYPE			render_target_type;

	typedef ::D2D1_FACTORY_TYPE					factory_type;
	typedef ::ID2D1Factory						factory;
}

#endif /* !EWIN_DRAWING_TYPE_ALIASES_H */
