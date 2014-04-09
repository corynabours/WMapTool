#include "stdafx.h"
#include "Library.h"
#include "Application.h"
#include "Preferences.h"
#include "Resource.h"

Library* Library::_instance = 0;

Library::Library()
{
}


Library::~Library()
{
}

Library * Library::Instance()
{
	if (_instance == 0)
	{
		_instance = new Library();
		_instance->Initialize(L"MapTool-Library", IDS_LIBRARY_TITLE,L"Library");
	}
	return _instance;
}


