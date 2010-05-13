#include "object.h"

struct TypeList {
    enum ClassType type;
    TypeList next;
};

Object
Object_ctor(Object o, TypeList types)
{
    types = RegisterType(types, CObject);
    o->types = types;
}

enum ClassType
GetType(void const *object)
{
    return GetObject(object)->types->type;
}

Object
GetObject(void const *object)
{
    return (Object)object;
}

Object
GetObjectOf(void const *object, const enum ClassType T)
{
    Object p = GetObject(object);
    TypeList types = p->types;
    while (types)
    {
	if (types->type == T) return p;
	types = types->next;
    }
    return 0;
}

TypeList
RegisterType(TypeList types, const enum ClassType T)
{
    TypeList type = XMALLOC(struct TypeList, 1);
    type->type = T;
    if (!types) return type;
    TypeList tail = types;
    while (tail->next) tail = tail->next;
    tail->next = type;
    return types;
}

void
FreeTypeList(TypeList types)
{
    if (types->next) FreeTypeList(types->next);
    XFREE(types);
}

void
Object_dtor(Object o)
{
    FreeTypeList(o->types);
}

