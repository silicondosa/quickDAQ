/*!
 * \file cLinkedList.c
 * \brief cLinkedList: A simple C library that implements a doubly linked list.
 * \author Suraj Chakravarthi Raja
 */


#include "cLinkedList.h"
#include "macrodef.h"
#include <stdio.h>

#ifdef _WIN32
    #include <cstddef>
    #include <cstdlib>
#else
    #include <stddef.h>
    #include <stdlib.h>
#endif


/********************************/
/* Linked list status functions */
/********************************/

/*!
 * \fn unsigned long cListLength (cLinkedList *myList)
 * Returns the length of the ::cLinkedList linked list passed to it as a parameter.
 * 
 * \param myList ::cLinkedList pointer of the linked list whose length is returned.
 * \return Length of the linked list passed as a parameter. Returns 0 if myList is a null pointer as well.
 */
unsigned long cListLength (cLinkedList *myList)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListLength): WARNING - Quashed attempt to access NULL pointer.\n");
		return 0;
	}
    return myList->num_elem;
}

/*!
 * \fn int cListEmpty (cLinkedList *myList) 
 * Checks to see if the ::cLinkedList linked list passed a parameter is empty.
 * 
 * \param myList ::cLinkedList pointer of the linked list which we need to check if empty.
 * \return Returns (bool) TRUE if the passed linked list is empty and (bool) FALSE otherwise.
 */
int cListEmpty (cLinkedList *myList)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListEmpty): WARNING - Quashed attempt to access NULL pointer.\n");
		return FALSE;
	}else if (myList->num_elem > 0) {
        return FALSE;
    }
    return TRUE;
}


/******************************/
/* Linked list read functions */
/******************************/
/*!
 * \fn cListElem * cListFirstElem (cLinkedList *myList)
 * Returns a ::cListElem pointer to the first element on the linked list passed as a parameter.
 * Returns a NULL pointer if the list is empty.
  * 
 * \param myList ::cLinkedList pointer to the linked list whose first element we need.
 * \return ::cListElem pointer to the first element of the linked list.
 */
cListElem * cListFirstElem (cLinkedList *myList)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListFirstElem): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	}
	return myList->anchor.nextElem;
}

/*!
 * \fn cListElem * cListLastElem (cLinkedList *myList)
 * Returns a ::cListElem pointer to the last element of the linked list passed as a paramener.
 * Returns a NULL pointer if the list is empty.
 * 
 * \param myList ::cLinkedList pointer to the linked list whose last element we need.
 * \return ::cListElem pointer to the last element of the linked list. If myList is a null pointer, so will the output.
 */
cListElem * cListLastElem (cLinkedList *myList)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListLastElem): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	}
	return myList->anchor.prevElem;
}

/*!
 * \fn cListElem * cListNextElem (cLinkedList *myList, cListElem *myElem)
 * Returns a ::cListElem pointer to the element in a ::cLinkedList linked list that is next to and after another element.
 * Returns a NULL pinter if the list is empty or the element being passed as a pointer is the last element.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be read.
 * \param myElem The function returns the element next to the element pointed to by this ::cListElem pointer.
 * \return ::cListElem pointer to the element next to the element passed as a parameter. If myList or myElem is a null pointer, so will the output.
 */
cListElem * cListNextElem (cLinkedList *myList, cListElem *myElem)
{
	if (myList == NULL || myElem == NULL) {
		if (myList == NULL)
			fprintf(ERRSTREAM, "cLinkedList (cListNextElem): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	} else if ((myList->num_elem == 0) || (myList->anchor.prevElem == myElem)) {
        return NULL;
    }
    return myElem->nextElem;
}

/*!
 * \fn cListElem * cListPrevElem (cLinkedList *myList, cListElem *myElem)
 * Returns a ::cListElem pointer to the element in a ::cLinkedList linked list that is before another element.
 * Returns a NULL pinter if the list is empty or the element being passed as a pointer is the first element.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be read.
 * \param myElem The function returns the element beefore the element pointed to by this ::cListElem pointer.
 * \return ::cListElem pointer to the element that is before the element passed as a parameter. If myList or myElem is a null pointer, so will the output.
 */
cListElem * cListPrevElem (cLinkedList *myList, cListElem *myElem)
{
	if (myList == NULL || myElem == NULL) {
		if(myList == NULL)
			fprintf(ERRSTREAM, "cLinkedList (cListPrevElem): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	} else if ((myList->num_elem == 0) || (myList->anchor.nextElem == myElem)) {
        return NULL;
    }
    return myElem->prevElem;
}

/*!
 * \fn cListElem * cListFindElem (cLinkedList *myList, void *myData)
 * Attempts to find an element linked in a ::cLinkedList list using the data that would be held by the list element to be found.
 * Returns a ::cListElem pointer to the first element holding the data if the data is found in the list.
 * Returns a NULL pointer if the data isn't found.
 * The longer the list, the longer it could take to find the element. This is a linear search.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be read.
 * \param myData void pointer to the data that would be held by the list element that needs to be found.
 * \return ::cListElem pointer to the first element found that holds the data being used to find the element. If myList is a null pointer, so will the output.
 */
cListElem * cListFindElem (cLinkedList *myList, void *myData)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListFindElem): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	}
    cListElem *myElem = NULL;
    for (myElem = cListFirstElem(myList); myElem != NULL ; myElem = cListNextElem(myList, myElem)) {
        if (myElem->obj == myData) {
            return myElem;
        }
    }
    return myElem;
}

/*!
 * \fn void * cListFindData (cLinkedList *myList, void *myData)
 * Attempts to find an element linked in a ::cLinkedList list using the data that would be held by the list element to be found.
 * Returns a void pointer to the data found on the linked list.
 * Returns a NULL pointer if the data isn't found.
 * The longer the list, the longer it could take to find the element. This is a linear search.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be read.
 * \param myData void pointer to the data that would be held by the list element that needs to be found.
 * \return void pointer to the data found on the linked list. If myList is a null pointer, so will the output.
 */ 
void * cListFindData (cLinkedList *myList, void *myData)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListFindData): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	}
	cListElem *elem = cListFindElem(myList, myData);
	if (elem == NULL)
		return NULL;
	else
		return elem->obj;
}

/*!
 * \fn void * cListFirstData (cLinkedList *myList)
 * Returns a pointer to the data held by the first element on the ::cLinkedList list.
 * But, it returns a NULL pointer if the list is empty.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \return void pointer to the data held by the first element in the linked list passed as a pointer. If myList is a null pointer, so will the output.
 */
void * cListFirstData (cLinkedList *myList)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListFirstData): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	}
	cListElem *elem = (cListFirstElem(myList));
	if (elem == NULL)
		return NULL;
	else
		return elem->obj;
}

/*!
 * \fn void * cListLastData (cLinkedList *myList)
 * Returns a pointer to the data held by the last element on the ::cLinkedList list.
 * But, it returns a NULL pointer if the list is empty.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \return void pointer to the data held by the last element in the linked list passed as a pointer. If myList is a null pointer, so will the output.
 */
void * cListLastData (cLinkedList *myList)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListLastData): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	}
	cListElem *elem = (cListLastElem(myList));
	if (elem == NULL)
		return NULL;
	else
		return elem->obj;
}

/*!
 * \fn void * cListNextData (cLinkedList *myList, void *myData)
 * Returns a pointer to the data next to the data on the ::cLinkedList list as passed to the function.
 * But, it returns a NULL pointer if the list is empty or if the passed data does not exist.
 * 
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \param myData void pointer to the data next to which we get the want look for the next data.
 * \return void pointer to the data held by the next element in the linked list passed as a pointer. If myList is a null pointer, so will the output.
 */
void * cListNextData (cLinkedList *myList, void *myData)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListNextData): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	}
	cListElem *elem = cListNextElem(myList, cListFindElem(myList, myData));
	if (elem == NULL)
		return NULL;
	else
		return elem->obj;
}

/*!
 * \fn void * cListPrevData (cLinkedList *myList, void *myData)
 * Returns a pointer to the data before the data on the ::cLinkedList list as passed to the function.
 * But, it returns a NULL pointer if the list is empty or if the passed data does not exist.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \param myData void pointer to the data before which we look for the previous data.
 * \return void pointer to the data held by the previous element in the linked list passed as a pointer. If myList or is a null pointer, so will the output.
 */
void * cListPrevData(cLinkedList *myList, void *myData)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListPrevData): WARNING - Quashed attempt to access NULL pointer.\n");
		return NULL;
	}
	cListElem *elem = cListPrevElem(myList, cListFindElem(myList, myData));
	if (elem == NULL)
		return NULL;
	else
		return elem->obj;
}


/**********************************/
/* Linked list creation functions */
/**********************************/
/*!
 * \fn int cListInit (cLinkedList *myList)
 * Dynamically initialized the ::cLinkedList pointer with a linked list.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \return 1 if the linked list was succesfully iniitialized and 0 if it wasn't.
 */
int cListInit (cLinkedList *myList)
{
    if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListInit): WARNING - Quashed attempt to access NULL pointer.\n");
        return 0;
    }

    myList->num_elem        = 0;

    myList->anchor.prevElem = NULL;
    myList->anchor.nextElem = NULL;
    myList->anchor.obj      = NULL;

    return 1;
}

/*!
 * \fn int cListInsertAfter (cLinkedList *myList, void *newData, cListElem *elem)
 * Inserts the data element passed into the linked list specified as a list element after the specified list element.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \param newData void pointer to the new data  to be inserted.
 * \param elem ::cListElem pointer to the element next to which the new data needs to be inserted
 * \return Upon successful insertion, the function returns a (int) 1. 
 */
int cListInsertAfter (cLinkedList *myList, void *newData, cListElem *elem)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListInsertAfter): WARNING - Quashed attempt to access NULL pointer.\n");
		return 0;
	}
	cListElem *newElem = (cListElem *)malloc(sizeof(cListElem));
    if (cListEmpty(myList)) {
		myList->anchor.prevElem = newElem;
		myList->anchor.nextElem = newElem;
		newElem->prevElem		= newElem;
		newElem->nextElem		= &(myList->anchor);
    } else {
        newElem->prevElem           = elem;
        newElem->nextElem           = elem->nextElem;
        (elem->nextElem)->prevElem  = newElem;
         elem->nextElem             = newElem;
    }
    newElem->obj      = newData;
    myList->num_elem += 1;
    return 1;
}

/*!
 * \fn int cListInsertBefore (cLinkedList *myList, void *newData, cListElem *elem)
 * Inserts the data element passed into the linked list specified as a list element before the specified list element.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \param newData void pointer to the new data to be inserted.
 * \param elem ::cListElem pointer to the element before which the new data needs to be inserted
 * \return Upon successful insertion, the function returns a (int) 1.
 */
int cListInsertBefore (cLinkedList *myList, void *newData, cListElem *elem)
{
	if (myList == NULL) {
		fprintf(ERRSTREAM, "cLinkedList (cListInsertAfter): WARNING - Quashed attempt to access NULL pointer.\n");
		return 0;
	}
    cListElem *newElem = (cListElem *)malloc(sizeof(cListElem));
    if (cListEmpty(myList)) {
		myList->anchor.prevElem = newElem;
		myList->anchor.nextElem = newElem;
		newElem->prevElem		= newElem;
		newElem->nextElem		= &(myList->anchor);
    } else {
        newElem->prevElem           = elem->prevElem;
        newElem->nextElem           = elem;
        (elem->prevElem)->nextElem  = newElem;
        elem->prevElem              = newElem;
    }
    newElem->obj      = newData;
    myList->num_elem += 1;
    return 1;
}

/*!
 * \fn int cListAppend (cLinkedList *myList, void *newData)
 * Inserts the specified new data at the end of the supplied linked list as the last element.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \param newData void pointer to the new data to be inserted.
 * \return Upon successful insertion, the function returns a (int) 1.
 */
int cListAppend (cLinkedList *myList, void *newData)
{
    return cListInsertAfter(myList, newData, myList->anchor.prevElem);
}

/*!
 * \fn int cListPrepend (cLinkedList *myList, void *newData) {
 * Inserts the specified new data at the beginning of the supplied linked list as the first element.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \param newData void pointer to the new data to be inserted.
 * \return Upon successful insertion, the function returns a (int) 1.
 */
int cListPrepend (cLinkedList *myList, void *newData) {
    return cListInsertBefore(myList, newData, myList->anchor.nextElem);
}

/********************************/
/* Linked list unlink functions */
/********************************/
/*!
 * \fn  void cListUnlinkElem (cLinkedList *myList, cListElem *delElem)
 * Unlinks the element requested from the linked list if it exists in the linked list.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 * \param delElem ::cListElem pointer to the element that needs to unlinked.
 */
 void cListUnlinkElem (cLinkedList *myList, cListElem *delElem)
{
    if (myList->num_elem > 0 && delElem != NULL) {
        (delElem->prevElem)->nextElem = delElem->nextElem;
        (delElem->nextElem)->prevElem = delElem->prevElem;
        myList->num_elem -= 1;

        free(delElem);
        delElem = NULL;
    } else if (delElem == NULL) {
        fprintf(ERRSTREAM, "cListUnlinkElem (cLinkedList): WARNING - Quashed attempt to de-reference a NULL pointer!\n");
    } else {
        fprintf(ERRSTREAM, "cListUnlinkElem (cLinkedList): WARNING - Quashed attempt to access empty linked list.\n");
    }

    if (cListEmpty(myList)) {
        cListInit(myList);
    }
}

/*!
 * \fn void cListUnlinkAll (cLinkedList *myList)
 * Unlinks all elements from the linked list and empties the ::cLinkedList list.
 * Calling this function before the end of the program ensure that the linked list does not leave any orphaned memory locations when the program exits.
 * 
 * \param myList ::cLinkedList pointer to the linked list need to be used.
 */
void cListUnlinkAll (cLinkedList *myList)
{
	cListElem *elem = cListFirstElem(myList);
	cListElem *nextElem = cListNextElem(myList, elem);

    while (elem != NULL) {
		cListUnlinkElem(myList, elem);
		elem = nextElem;
		nextElem = cListNextElem(myList, elem);
	}
}

/*!
 * \fn void cListUnlinkData (cLinkedList *delList, void *delData)
 * Unlinks the first element in the list which holds the data pointed to by this pointer.
 * 
 * \param delList ::cLinkedList pointer to the linked list need to be used. 
 * \param delData void pointer to the data whose element needs to be unlinked from the list.
 */
void cListUnlinkData (cLinkedList *delList, void *delData)
{
    cListUnlinkElem(delList, cListFindElem(delList, delData));
}