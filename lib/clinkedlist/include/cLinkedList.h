/*!
 * \file cLinkedList.h
 * \brief cLinkedList: A simple linked list manager for C/C++.
 * \author Suraj Chakravarthi Raja
 */

#pragma once

#ifndef CLINKEDLIST_H
#define CLINKEDLIST_H

#ifdef __cplusplus 
extern "C" {
#endif 

#include <stddef.h>
#include "macrodef.h"

 /*!
 * \typedef Structure for a single element on the ::cLinkedList linked list.
 */
typedef struct _cListElem {
	/*! Data object pointed to the list element.*/
    void    *obj;
	/*! The element next to this element on the list.*/
    struct  _cListElem *nextElem;
	/*! The element before this element on the list.*/
    struct  _cListElem *prevElem;
} cListElem;

/*!
 * Structure for a linked list of ::cListElem elements.
 */
typedef struct _cLinkedList {
	/*! Number of elements in the linked list.*/
    unsigned long        num_elem;
	/*! The achhor element of the linked list to which all other elements are tied.*/
    cListElem   anchor;
} cLinkedList;

// Linked list status functions

/*!
 * Returns the length of a linked list.
 * 
 * \param myList ::cLinkedList pointer of the linked list whose length is returned.
 * \return Length of the linked list passed as a parameter. Returns 0 if myList is a null pointer as well.
 */
unsigned long	cListLength	(cLinkedList *myList);

/**
 * Returns 1 if a linked list is empty.
 * @param myList Pointer to the \ref cLinkedList linked list which needs to be checked.
 * @return Returns (int) 1 if the linked list is empty and 0 otherwise.
*/
int				cListEmpty	(cLinkedList *myList);

// Linked list read functions

/**
 * @brief Returns the first element of a linked list.
 * @param myList Pointer to the \ref cLinkedList linked list whose first element is needed.
 * @return Pointer (of type \ref cListElem) to the first element of the linked list. Returns NULL if empty.
*/
cListElem * cListFirstElem   (cLinkedList *myList);

/**
 * @brief Returns the last element of a linked list.
 * @param myList Pointer to the \ref cLinkedList linked list whose last element is needed.
 * @return Pointer (of type \ref cListElem) to the last element of the linked list. Returns NULL if empty.
*/
cListElem * cListLastElem    (cLinkedList *myList);

/**
 * @brief Returns the next element of a linked list when provided an element from the list.
 * @param myList Pointer to the \ref cLinkedList linked list being accessed.
 * @param myElem Pointer to the \ref cListElem element in the list whose next element is needed.
 * @return Pointer (of type \ref cListElem) to the next element of the linked list if it exists. Returns NULL otherwise.
*/
cListElem * cListNextElem    (cLinkedList *myList, cListElem *myElem);

/**
 * @brief Returns the previous element of a linked list, when provided an element from the list.
 * @param myList Pointer to the \ref cLinkedList linked list being accessed.
 * @param myElem Pointer to the \ref cListElem element in the list whose previous element is needed.
 * @return Pointer (of type \ref cListElem) to the previous element of the linked list if it exists. Returns NULL otherwise.
*/
cListElem * cListPrevElem    (cLinkedList *myList, cListElem *myElem);

/**
 * @brief Find an element in a linked list passed using the data linked in it.
 * @param myList Pointer to the \ref cLinkedList linked list being accessed.
 * @param myElem Pointer to the data which is linked to the element being search for.
 * @return Void pointer to element of the linked list if the data is linked in the list. NULL returned otherwise.
*/
cListElem * cListFindElem    (cLinkedList *myList, void *myData);

/**
 * @brief Find a data point in a linked list if it is present.
 * @param myList Pointer to the \ref cLinkedList linked list being accessed.
 * @param myElem Pointer to the data being searched for.
 * @return Void pointer to the data of the linked list if the data is found. NULL returned otherwise.
*/
void *      cListFindData    (cLinkedList *myList, void *myData);

/**
 * @brief Returns the first data point of a linked list.
 * @param myList Pointer to the \ref cLinkedList linked list whose first data point is needed.
 * @return Void pointer to the first data point of the linked list. Returns NULL if empty.
*/
void * cListFirstData   (cLinkedList *myList);

/**
 * @brief Returns the last data point of a linked list.
 * @param myList Pointer to the \ref cLinkedList linked list whose last data point is needed.
 * @return Void pointer to the last data point of the linked list. Returns NULL if empty.
*/
void * cListLastData    (cLinkedList *myList);

/**
 * @brief Returns the next data point of a linked list when provided a data point from the list.
 * @param myList Pointer to the \ref cLinkedList linked list being accessed.
 * @param myElem Pointer to the data point in the list whose next data point is needed.
 * @return Void pointer to the next data point of the linked list if both data points exist. Returns NULL otherwise.
*/
void * cListNextData    (cLinkedList *myList, void *myData);

/**
 * @brief Returns the previous data point of a linked list, when provided a data point from the list.
 * @param myList Pointer to the \ref cLinkedList linked list being accessed.
 * @param myElem Pointer to the data point in the list whose previous data point is needed.
 * @return Void pointer to the previous data point the linked list if both data points exist. Returns NULL otherwise.
*/
void * cListPrevData    (cLinkedList *myList, void *myData);

// Linked list creation functions

/**
 * @brief Initializes a doubly linked list.
 * @param myList Pointer to the \ref cLinkedList linked list which has already been created.
 * @return Returns 1 if the successful, and 0 otherwise.
*/
int cListInit                (cLinkedList *myList);

/**
 * @brief Insert a data point into a linked list after an existing element in the list.
 * @param myList Pointer to the \ref cLinkedList linked list to which data needs to be linked.
 * @param myData Void pointer to the data point to be linked as an element into the linked list.
 * @param myElem Pointer to the \ref cListElem element in the linked list after which the new data point needs to be inserted.
 * @return Returns 1 if successul and 0 otherwise.
*/
int cListInsertAfter     (cLinkedList *myList, void *myData, cListElem *myElem);

/**
 * @brief Insert a data point into a linked list before an existing element in the list.
 * @param myList Pointer to the \ref cLinkedList linked list to which data needs to be linked.
 * @param myData Void pointer to the data point to be linked as an element into the linked list.
 * @param myElem Pointer to the \ref cListElem element in the linked list before which the new data point needs to be inserted.
 * @return Returns 1 if successful and 0 otherwise.
*/
int cListInsertBefore    (cLinkedList *myList, void *myData, cListElem *myElem);

/**
 * @brief Append a data point into a linked list.
 * @param myList Pointer to the \ref cLinkedList linked list to which data needs to be linked.
 * @param myData Void pointer to the data point to be linked as an element into the linked list.
 * @return Returns 1 if successful and 0 otherwise.
*/
int cListAppend          (cLinkedList *myList, void *myData);

/**
 * @brief Prepend a data point into a linked list
 * @param myList Pointer to the \ref cLinkedList linked list to which data needs to be linked.
 * @param myData Void pointer to the data point to be linked as an element into the linked list.
 * @return Returns 1 if successful and 0 otherwise.
*/
int cListPrepend         (cLinkedList *myList, void *myData);

// Linked list unlink functions

/**
 * @brief Unlinks an element from a linked list. This does not remove the actual data point linked.
 * @param myList Pointer to the \ref cLinkedList linked list from which an element needs to be unlinked.
 * @param myElem Pointer to the \ref cListElem in the linked list that needs to be unlinked.
*/
void cListUnlinkElem         (cLinkedList *myList, cListElem *myElem);

/**
 * @brief Unlinks all the elements from a linked list. This does not remove the actual data points linked.
 * @param myList Pointer to the \ref cLinkedList linked list which needs to be emptied out in full.
*/
void cListUnlinkAll          (cLinkedList *myList);

/**
 * @brief Unlinks a data point that is linked in a linked list.
 * @param myList Pointer to the \ref cLinkedList linked list from which a data point needs to be unlinked.
 * @param myData Void pointer to the data point which needs to be unlinked from the linked list.
*/
void cListUnlinkData         (cLinkedList *myList, void *myData);

#ifdef __cplusplus 
}
#endif 

#endif //CLINKEDLIST_H
