#pragma once
#include "DockingWindow.h"
class Library : DockingWindow
{
private:
	static Library *_instance;
public:
	static Library *Instance();
protected:
	Library();
	~Library();
};

