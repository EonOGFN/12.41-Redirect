#pragma once
#include <array>

namespace raax_patch {
	constexpr std::array<uint64_t, 3> RAAX_CRASH_PATCH_OFFSETS = {
		0x45A6BA0, //k2_drawbox
		0x45A6C60, //k2_drawline
		0x45A77D0 //k2_project
	};
}