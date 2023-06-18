#include "fab_utility.h"



void Utl_PushFrontList(Utl_List* list, Utl_ListLinks* node)
{
	Utl_Assert(list);
	Utl_Assert(node);
	if (list->begin == NULL)
	{
		node->prev = NULL;
		node->next = NULL;
		list->begin = node;
		list->end = node;
	} else
	{
		node->prev = NULL;
		node->next = list->begin;
		list->begin->prev = node;
		list->begin = node;
	}
	list->count++;
}

void Utl_PushBackList(Utl_List* list, Utl_ListLinks* node)
{
	Utl_Assert(list);
	Utl_Assert(node);
	if (list->begin == NULL)
	{
		node->prev = NULL;
		node->next = NULL;
		list->begin = node;
		list->end = node;
	} else
	{
		node->prev = list->end;
		node->next = NULL;
		list->end->next = node;
		list->end = node;
	}
	list->count++;
}

void* Utl_PopFrontList(Utl_List* list)
{
	Utl_ListLinks* node = list->begin;
	Utl_UnlinkFromList(list, node);
	return node;
}

void* Utl_PopBackList(Utl_List* list)
{
	Utl_ListLinks* node = list->end;
	Utl_UnlinkFromList(list, node);
	return node;
}

void Utl_UnlinkFromList(Utl_List* list,
						Utl_ListLinks* node)
{
	if (node->prev)
		node->prev->next = node->next;
	else
		list->begin = node->next;

	if (node->next)
		node->next->prev = node->prev;
	else
		list->end = node->prev;

	node->prev = NULL;
	node->next = NULL;

	list->count--;
}

void Utl_ClearList(Utl_List* list, void(*destroy_func)(void*))
{
	if (!list) return;
	if (list->count <= 0) return;
	if (list->begin == NULL) return;
	if (list->end == NULL) return;

	Utl_ListLinks* iterator = list->begin;
	while (iterator)
	{
		Utl_ListLinks* delblock = iterator;
		iterator = iterator->next;
		if (destroy_func)
			destroy_func(delblock);
		else
			Utl_Free(delblock);
	}
	list->begin = NULL;
	list->end = NULL;
	list->count = 0;
}



char* Utl_CopyCutString(const char* srcstring, const int start, const int length)
{
	Utl_Assert(srcstring);
	Utl_Assert(start >= 0);
	Utl_Assert(length >= 0);
#ifdef Utl_DEBUG
	/* Make sure the bounds of srcstring are not exceeded */
	size_t srcstring_len = (int)strlen(srcstring);
	Utl_Assert(srcstring_len >= 0);
	if (start + length > srcstring_len) Utl_Assert(0);
#endif

	char* str = Utl_Malloc(length + 1); /* Plus 1 to include null terminator */
	for (int i = 0; i < length; i++)
		str[i] = srcstring[i + start];
	str[length] = '\0';
	return str;
}
