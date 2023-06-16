/**
 * fab_utility.h - Basic utilities for all Fabric code
 * 
 * This code should eventually be shared across all executables.
 */

#ifndef _Lnn_UTILITY_H_
#define _Lnn_UTILITY_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>

#ifdef _DEBUG
#define Utl_DEBUG
#endif



/**
 * Boolean type
 * May be replaced by built in bool type
 */
typedef enum
{
	Utl_FALSE = 0,
	Utl_TRUE = 1
} Utl_Bool;



#ifdef Utl_DEBUG
#define Utl_Assert(expression) assert(expression)
#else
#define Utl_Assert(expression) void
#endif



#ifndef Utl_USE_64BIT_NUMBERS
typedef float Utl_Float;
typedef int32_t Utl_Int;
#define Utl_StringToFloat strtof
#else
typedef double Utl_Float;
typedef int64_tUtl_Int;
#define Utl_StringToFloat strtod
#endif



/* Compile without using Fabric wrappers for allocating memory */
#define Utl_USE_STD_ALLOC
#ifdef Utl_USE_STD_ALLOC

#define Utl_Malloc(size)			malloc(size)
#define Utl_Calloc(count, size)		calloc(count, size)
#define Utl_Realloc(block, size)	realloc(block, size)
#define	Utl_AllocType(type)			(type*)calloc(1, sizeof(type))
#define Utl_Free(block)				free(block)

#else

void* Lnn_Malloc(const size size);
void* Lnn_Calloc(const int count, const size size);
void* Lnn_Realloc(void* block, const size size);
void Lnn_Free(void* block);
#define Lnn_MALLOC(size)			Lnn_Malloc(size)
#define Lnn_CALLOC(count, size)		Lnn_Calloc(count, size)
#define Lnn_REALLOC(block, size)	Lnn_Realloc(block, size)
#define	Lnn_ALLOC_TYPE(type)		Lnn_Calloc(1, sizeof(type))
#define Lnn_FREE(block)				Lnn_Free(block)

#endif



/* Double linked list implementation */

/**
 * @brief Struct holding the links of a node in a doubly linked list.
 */
typedef struct Utl_ListLinks
{
	struct Utl_ListLinks* prev;
	struct Utl_ListLinks* next;
} Utl_ListLinks;

typedef struct
{
	Utl_ListLinks*	begin;
	Utl_ListLinks*	end;
	int				count;
} Utl_List;

/**
 * @brief Pushes an element onto the beginning of a list.
 * @param list The list to push the element onto.
 * @param node Pointer to the element to push.
 */
void Utl_PushFrontList(Utl_List*		list,
					   Utl_ListLinks*	node);

/**
 * @brief Pushes an element onto the end of a list.
 * @param list The list to push the element onto.
 * @param node Pointer to the element to push.
 */
void Utl_PushBackList(Utl_List*			list,
					  Utl_ListLinks*	node);

/**
 * @brief Removes all elements from a list and calling the destroy_func on every element.
 * This does not free the list itself, only empties it.
 * @param list Pointer to the list to empty.
 * @param destroy_func Pointer to a destructor function, or NULL to use Lnn_Free().
 */
void Utl_ClearList(Utl_List* list,
				   void(*destroy_func)(void*));



/* String functions */

#define Utl_Stringify2(str) #str
#define Utl_Stringify(str) Utl_Stringify2(str)

#define Utl_CharIsInvalid(c) (c < 0)

/**
 * @brief Copies part of a string and returns a new string of that.
 * @param srcstring String to copy from.
 * @param start Where in the srcstring to start.
 * @param length The length of the part to copy.
 * @return Pointer to the new string, remember to free!
 */
char* Utl_CopyCutString(
	const char* srcstring,
	const int start,
	const int length
);

#endif
