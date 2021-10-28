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
 * \fn unsigned long cListLength (cLinkedList *myList)
 * Returns the length of the ::cLinkedList linked list passed to it as a parameter.
 * 
 * \param myList ::cLinkedList pointer of the linked list whose length is returned.
 * \return Length of the linked list passed as a parameter. Returns 0 if myList is a null pointer as well.
 */
unsigned long	cListLength	(cLinkedList *myList);

int				cListEmpty	(cLinkedList *myList);

// Linked list read functions
cListElem * cListFirstElem   (cLinkedList *myList);
cListElem * cListLastElem    (cLinkedList *myList);
cListElem * cListNextElem    (cLinkedList *myList, cListElem *myElem);
cListElem * cListPrevElem    (cLinkedList *myList, cListElem *myElem);

cListElem * cListFindElem    (cLinkedList *myList, void *myData);
void *      cListFindData    (cLinkedList *myList, void *myData);

void * cListFirstData   (cLinkedList *myList);
void * cListLastData    (cLinkedList *myList);
void * cListNextData    (cLinkedList *myList, void *myData);
void * cListPrevData    (cLinkedList *myList, void *myData);

// Linked list creation functions
int cListInit                (cLinkedList *myList);

int cListInsertAfter     (cLinkedList *myList, void *myData, cListElem *myElem);
int cListInsertBefore    (cLinkedList *myList, void *myData, cListElem *myElem);
int cListAppend          (cLinkedList *myList, void *myData);
int cListPrepend         (cLinkedList *myList, void *myData);

// Linked list unlink functions
void cListUnlinkElem         (cLinkedList *myList, cListElem *myElem);

void cListUnlinkAll          (cLinkedList *myList);

void cListUnlinkData         (cLinkedList *myList, void *myData);

#ifdef __cplusplus 
}
#endif 

#endif //CLINKEDLIST_H
