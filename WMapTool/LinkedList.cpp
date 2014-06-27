#include "stdafx.h"
#include "LinkedList.h"


void LinkedList::InsertAfter(LinkedList *newNode){
	if (next == NULL)
	{
		next = newNode;
	}
	else
	{
		newNode->next = next;
		next = newNode;
	}
}

void LinkedList::AddToEnd(void *newData)
{
	LinkedList *lastNode = this;
	while (lastNode->next != NULL)
		lastNode = lastNode->next;
	LinkedList *newNode = new LinkedList(newData);
	lastNode->next = newNode;
}

LinkedList::LinkedList(void *newdata)
{
	data = newdata;
}

LinkedList::~LinkedList()
{
}
