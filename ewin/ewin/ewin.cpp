#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "common/type_aliases.h"
#include "common/raii.h"
#include "common/integral_property.h"
#include "common/boolean_property.h"
#include "common/string_property.h"
#include "common/size_property.h"
#include "common/rect_property.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){
	int v1 = 0, v2 = 9, v3 = 45;

	ewin::common::integral_value_property<int> iprop(v1);
	ewin::common::read_only_integral_value_property<int> roiprop(v2);
	ewin::common::write_only_integral_value_property<int> woiprop(v3);

	iprop = roiprop;
	int ival = iprop;
	iprop += 18;
	auto sub = iprop - 63;
	auto rsub = 99 - iprop;

	roiprop = 72;
	int wval = woiprop;
	auto common = roiprop * (int)woiprop;

	ewin::common::boolean_value_property<> bprop;
	ewin::common::read_only_boolean_value_property<> robprop;
	ewin::common::write_only_boolean_value_property<> wobprop;

	return 0;
}
