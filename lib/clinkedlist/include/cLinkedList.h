/*!
 * \file cLinkedList.h
 * \brief cLinkedList: A simple C library that implements a doubly linked list.
 * \author Suraj Chakravarthi Raja
 */

#pragma once

#ifndef CLINKEDLIST_H
#define CLINKEDLIST_H

#include "macrodef.h"

#ifdef _WIN32
    #include <stddef.h>
    #include <stdlib.h>
#else
    #include <stddef.h>
    #include <stdlib.h>
#endif

/*!
 * Structure for a single element on the ::cLinkedList linked list.
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
unsigned long	cListLength	(cLinkedList *);
int				cListEmpty	(cLinkedList *);

// Linked list read functions
cListElem * cListFirstElem   (cLinkedList *);
cListElem * cListLastElem    (cLinkedList *);
cListElem * cListNextElem    (cLinkedList *, cListElem *);
cListElem * cListPrevElem    (cLinkedList *, cListElem *);

cListElem * cListFindElem    (cLinkedList *, void *);
void *      cListFindData    (cLinkedList *, void *);

void * cListFirstData   (cLinkedList *);
void * cListLastData    (cLinkedList *);
void * cListNextData    (cLinkedList *, void *);
void * cListPrevData    (cLinkedList *, void *);

// Linked list creation functions
int cListInit                (cLinkedList *);

int cListInsertAfter     (cLinkedList *, void *, cListElem *);
int cListInsertBefore    (cLinkedList *, void *, cListElem *);
int cListAppend          (cLinkedList *, void *);
int cListPrepend         (cLinkedList *, void *);

// Linked list unlink functions
void cListUnlinkElem         (cLinkedList *, cListElem *);

void cListUnlinkAll          (cLinkedList *);

void cListUnlinkData         (cLinkedList *, void *);

#endif //CLINKEDLIST_H
