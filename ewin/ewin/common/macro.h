#pragma once

#ifndef EWIN_MACRO_H
#define EWIN_MACRO_H

#define EWIN_MAKE_OPERATORS_(x, t)\
	inline constexpr x operator |(x l, x r){\
		return (x)((t)(l) | (t)(r));\
	}\
\
	inline constexpr x operator &(x l, x r){\
		return (x)((t)(l) & (t)(r));\
	}\
\
	inline constexpr x operator ~(x r){\
		return (x)(~(t)(r));\
	}\
\
	inline x operator |=(x &l, x r){\
		return (l = (x)((t)(l) | (t)(r)));\
	}\
\
	inline x operator &=(x &l, x r){\
		return (l = (x)((t)(l) & (t)(r)));\
	}\
\
	inline x operator <<(x &l, t r){\
		return (x)((t)(l) << (r));\
	}\
\
	inline x operator >>(x &l, t r){\
		return (x)((t)(l) >> (r));\
	}

#define EWIN_MAKE_OPERATORS(x) EWIN_MAKE_OPERATORS_(x, unsigned int)

#define EWIN_MAKE_OPERATORS_LL(x) EWIN_MAKE_OPERATORS_(x, unsigned long long)

#define EWIN_SET(source, target) ((source) |= (target))
#define EWIN_REMOVE(source, target) ((source) &= ~(target))
#define EWIN_FILTER(source, target) ((source) &= (target))

#define EWIN_SET_V(source, target) ((source) | (target))
#define EWIN_REMOVE_V(source, target) ((source) & ~(target))
#define EWIN_FILTER_V(source, target) ((source) & (target))

#define EWIN_IS(source, target) (((source) & (target)) == (target))
#define EWIN_IS_ANY(source, target) (((source) & (target)) != static_cast<decltype(target)>(0))

#endif /* !EWIN_MACRO_H */
