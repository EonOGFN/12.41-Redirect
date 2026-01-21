#pragma once
#pragma once
#include <atomic>

constexpr unsigned long long START_LOGIN_OFFSET = 0xF81620;

namespace ensure_init {
	void init();

	void wait_for_init();

	void set_init();

	inline std::atomic<bool> m_IsInit;

	inline void* m_FunctionAddress;
}