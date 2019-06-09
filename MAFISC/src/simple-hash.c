/*
* Copyright (c) 2011 Nathanael Hübbe
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#include "config.h"

#define _GNU_SOURCE

#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "simple-hash.h"

#define allocate(type, name, count) type* name = (type*)malloc(sizeof(type)*count)

typedef struct sh_TableEntry sh_TableEntry;

struct sh_TableEntry {
    unsigned long index;
    long value;
    sh_TableEntry* next;
};

struct SimpleHash {
    size_t tableSize;
    sh_TableEntry** table;
};

SimpleHash* sh_create(size_t size) {
    size_t i;
//    fprintf(stderr, "sh_create()\n");
    allocate(SimpleHash, me, 1);
    me->tableSize = size;
    me->table = (sh_TableEntry**)malloc(sizeof(sh_TableEntry*)*size);
    for(i = 0; i < size; i++) me->table[i] = 0;
    return me;
}

long sh_setValue(SimpleHash* me, unsigned long index, long *value) {
    sh_TableEntry** curPointer = &(me->table[index%me->tableSize]);
    long temp;
    for(; *curPointer; curPointer = &((*curPointer)->next) ) {
        if((*curPointer)->index == index) break;
    }
    if(!*curPointer) {
        *curPointer = (sh_TableEntry*)malloc(sizeof(sh_TableEntry));
        if(!*curPointer) return -1;
        (*curPointer)->index = index;
        (*curPointer)->value = *value;
        (*curPointer)->next = 0;
        return 0;
    }
    temp = (*curPointer)->value;
    (*curPointer)->value = *value;
    *value = temp;
    return 1;
}

long sh_retrieveValue(SimpleHash* me, unsigned long index, long* value) {
    sh_TableEntry** curPointer = &(me->table[index%me->tableSize]);
    for(; *curPointer; curPointer = &((*curPointer)->next) ) {
        if((*curPointer)->index == index) break;
    }
    if(!*curPointer) return -1;
    *value = (*curPointer)->value;
    return 0;
}

void sh_delete(SimpleHash* me) {
    size_t i;
    for(i = 0; i < me->tableSize; i++) {
        sh_TableEntry* curEntry = me->table[i];
        while(curEntry) {
            sh_TableEntry* next = curEntry->next;
            free(curEntry);
            curEntry = next;
        }
    }
    free(me);
}
