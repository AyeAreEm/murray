/*  dyn.h - v0.1

    Header only file for dynamic arrays

    To use implementation, it's on a per type basis. Write `dynimpl(type T, typename Tn);` where T is the type of the elements
    in the dynamic array and Tn is syntax safe name of the type. For example `dynimpl(const char*, string);`

    NOTE: all definitions can be used either like `fn_name(type T)` or `fn_nameT()` EXCEPT `dynimpl(T)`
    The main reason for this is to allow compound literals to work since they don't in macros

    Types include:
        DynHeader -> struct { size_t len; size_t cap; }
        Dyn(T) -> T* (NOTE: this is if you wish to show that a particular field or variable can work with dyn.h, feel free to just do T*)

    Interfacing:
        Declare a dynamic array
            T* arr = NULL;

        Access index of dynamic array
            arr[index]

    Documenation:

        dynheader:
            DynHeader *dynheader(Dyn(T) arr);
                Returns the header

        dynlen:
            size_t &dynlen(Dyn(T) arr);
                Returns a reference to the length (NOTE: this means doing `dynlen(arr)++` WILL update the len in the header)

        dyncap:
            size_t &dyncap(Dyn(T) arr);
                Returns a reference to the capacity (NOTE: this means doing `dyncap(arr)++` WILL update the cap in the header)

        dynfree:
            void dynfree(Dyn(T) arr);
                Frees the array

        dynpush:
            bool dynpush(typename Tn, Dyn(T) *arr, T item);
                Pushes an element to the end of the array
                (NOTE: This MAY change where the array pointer points to, therefore any pointer stored from the array may be invalid)

        dynpop:
            T dynpop(typename Tn, Dyn(T) arr);
                Pops off the last element in the array and returns it

        dynremove:
            T dynremove(typename Tn, Dyn(T) arr, size_t index);
                Removes an element by index and returns it

        dynclear:
            void dynclear(typename Tn, Dyn(T) arr);
                Clears the array elements (NOTE: This just sets .len to 0 so the elements still exist but will be overwritten)
*/

#ifndef DYN_H
#define DYN_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define Dyn(T) T*
#define dynheader(arr) ((DynHeader*)((arr)) - 1)
#define dynlen(arr) (dynheader((arr))->len)
#define dyncap(arr) (dynheader((arr))->cap)
#define dynfree(arr) (free(dynheader((arr))))
#define dynpush(Tn, arr, item) dynpush##Tn((arr), (item))
#define dynpop(Tn, arr) dynpop##Tn((arr))
#define dynremove(Tn, arr, index) dynremove##Tn((arr), (index))
#define dynclear(Tn, arr) dynclear##Tn((arr))

typedef struct DynHeader {
    size_t len;
    size_t cap;
} DynHeader;

#define dynimpl(T, Tn)\
    bool dyngrow##Tn(Dyn(T) *arr) {\
        DynHeader *header;\
        if (*arr == NULL) {\
            header = (DynHeader*)realloc(NULL, sizeof(DynHeader) + sizeof(T));\
            /* TODO: check if header == NULL */\
            header->len = 0;\
            header->cap = 1;\
        } else {\
            header = (DynHeader*)realloc(dynheader(*arr), dyncap(*arr) * 2 * sizeof(T) + sizeof(DynHeader));\
            header->cap *= 2;\
        }\
        /* TODO: check if header == NULL */\
        (*arr) = (Dyn(T))(header + 1);\
        return true;\
    }\
    bool dynpush##Tn(Dyn(T) *arr, T item) {\
        if (*arr == NULL || dynlen(*arr) + 1 >= dyncap(*arr)) {\
            if (!dyngrow##Tn(arr)) {\
                return false;\
            }\
        }\
        (*arr)[dynlen(*arr)++] = item;\
        return true;\
    }\
    T dynpop##Tn(Dyn(T) arr) {\
        return arr[dynlen(arr)--];\
    }\
    T dynremove##Tn(Dyn(T) arr, size_t index) {\
        T item = arr[index];\
        memmove(&arr[index], &arr[index + 1], (dynlen(arr) - index - 1) * sizeof(T));\
        dynlen(arr) -= 1;\
        return item;\
    }\
    void dynclear##Tn(Dyn(T) arr) {\
        dynlen(arr) = 0;\
    }
#endif // DYN_H
