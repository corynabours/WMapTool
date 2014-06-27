#pragma once
class LinkedList
{
public:
	void InsertAfter(LinkedList *newNode); 
	void AddToEnd(void *newData);
	void *data = NULL;
	LinkedList *next = NULL;
	LinkedList(void *newdata);
	~LinkedList();
};

