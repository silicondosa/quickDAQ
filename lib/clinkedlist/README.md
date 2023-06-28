# cLinkedList - A simple linked list manager for C/C++ 
This is a simple linked list manager for C that I've been using for a long while now. It may not be as optimized as Boost's libraries for linked lists, but it is very simple to use and has been tested rigorously.

This library allows you to dynamically create and manage doubly linked lists of elements that already exist in memory using pointers. This means that the actual data is untouched by this library which essentially manages a linked list of pointers to data.

## Supported platforms and binaries
In addition to the source, you can download `cLinkedList` as a precompiled library. It's currently *only available as a static library for Windows (WIN32, x64)*, but I will make a DLL available when I have a time away from academics.

At some point, I will also build shared and static libraries for 64-bit PC Linux.

## Usage
Download the repo and place it in your project directory. Be sure to add the `include` directory along with either the `bin/Win32/` or `bin/x64/` directory to your compile and linker paths respectively. You do not need the `source` and `cLinkedList` directories, both of which can be safely removed.

## Supported functionalities and documentation
Doxygen documentation for each of the functionalities below are linked with the functions. The Doxygen documentation home page is [here](https://silicondosa.github.io/cLinkedList/index.html).
### 1. Linked list creation/modification functions
- ***Initialize*** a linked list in dynamic memory ([cListInit](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#ad4ec01b040c21f49657f99fcd08ec059)).
- ***Insert a data item AFTER*** an existing linked list element ([cListInsertAfter](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a04b80d599f35d9cf1f48a7784713620b)).
- ***Insert a data item BEFORE*** an existing linked list element ([cListInsertBefore](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a749a48d1e4112d99bdd0602fc4eaf8f9)).
- ***Append a data item*** to the end of a linked list ([cListAppend](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#aa86b0722c1e31e36dc332d7a164bce8f)).
- ***Prepend a data item*** to the start of the linked list ([cListPrepend](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#ab5dbb656bcfb21316f131a08d9c4c51b)).

### 2. Linked list reading functions
- Get the ***first list element/data item*** of a linked list ([cListFirstElem](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a4f98cc93c4e604f5849845cf0767ab5f)/[cListFirstData](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a11a41a752f27901c27935be3c4b7d582)).
- Get the ***last list element/data item*** of a linked list ([cListLastElem](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a1126ce4d932d49bcdbf6275b30fe7391)/[cListLastData](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#ac25927653414ab346483b0c64389bb3a)).
- Find the ***next list element/data item*** to a specific element of a linked list ([cListNextElem](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a0d63e7329559869dc4ef6a87dea97964)/[cListNextData](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#aab133b04b3c0e8a6389be55ab11983d1)).
- Find the ***previous list element/data item*** to a specific element of a linked list ([cListPrevElem](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a600f99eb259af1b9e2ccbba23ca76092)/[cListPrevData](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a517405e38e43ee40c7437d7a5d62c06f)).
- ***Find the list element*** associated with a particular data item, if linked ([cListFindElem](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a18a09fd8aea8d86bb97e4cf573c21874)).
- ***Find the data item*** in a list if it has been linked ([cListFindData](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a325ea31f9f9bd7677bb7f2ade2a5392f)).

### 3. Linked list status functions
- ***Get the length*** of the linked list ([cListLength](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a63b0a9858d8981f31d13aaeb8a450a19)).
- Is the linked list ***empty?*** ([cListEmpty](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#a1795c176a0adc369cdff8b47573bd331))

### 4. Linked list unlinking functions
- ***Unlink a particular list element*** in a linked list ([cListUnlinkElem](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#afd3a34765ef5f801e0757e24135580d2)).
- ***Unlink a list element pointing to specific data item*** in a linked list ([cListUnlinkData](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#acd07bd1453b3f5cb11769c32f943d082)).
- ***Unlink all list elements*** from a linked list ([cListUnlinkAll](https://silicondosa.github.io/cLinkedList/c_linked_list_8h.html#aceb6e6abaec76d561d18cb216f55dfb3)).

This library is considered feature-complete for my purposes. If you see any bugs or want a feature implemented, please don't hesitate to open an issue or send pull request my way.

## Learning outcomes
POINTERS POINTERS POINTERS! First and foremost, creating this library was an exercise in the correct usage of C pointers. Additionally, this library gave the opportunity to write code that completely platform-agnostic and portable. With only one header and only one source file, it is easy to compile together with your other C/C++ code.

## Licensing
This library and its source code is distributed under the [Mozilla Public License v2.0](https://choosealicense.com/licenses/mpl-2.0/).
