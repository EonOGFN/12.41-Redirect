#include "ensure_init.h"
#include <memcury.h>
#include <defines.h>
#include <lazy_importer.hpp>

void ensure_init::init()
{
	ensure_init::m_IsInit.store(false);
}

void ensure_init::wait_for_init()
{
	//wait until m_IsInit is true

	while (!ensure_init::m_IsInit.load())
	{
		LI_FN(Sleep)(33);
	}

	LI_FN(Sleep)(1000);
}


void ensure_init::set_init()
{
	m_IsInit.store(true);
}