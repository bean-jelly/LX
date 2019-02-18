#include "gameloadhelper.h"
#include "basiccode/utility/BasicLoadHelper.h"
gameloadhelper::gameloadhelper()
{
	m_handle = NULL;
	m_pfn_create = NULL;
}

gameloadhelper::~gameloadhelper()
{
	utility::BasicLoadHelper::get_instance()->free_library(m_handle);
	m_handle = NULL;
	m_pfn_create = NULL;
}

gameloadhelper* gameloadhelper::get_instance()
{
	static gameloadhelper mgr;
	return &mgr;
}

int gameloadhelper::init_load(const char* dll_path)
{
	this->m_handle = utility::BasicLoadHelper::get_instance()->load_library(dll_path);
	return 0;
}

basegamelogic* gameloadhelper::create_logic()
{
	if (this->m_handle)
	{
		m_pfn_create = (pfnCreateGameInterface)utility::BasicLoadHelper::get_instance()->get_proc_address(this->m_handle, "CreateGameInterface");
		if (NULL != m_pfn_create)
		{
			return (basegamelogic*)m_pfn_create(1);
		}
	}
	return NULL;
}