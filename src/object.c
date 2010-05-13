#include "object.h"

struct TypeList {
    unsigned char n;
    Type type[16];
    TypeList next;
};

Object
Object_ctor(Object o, TypeList types)
{
    types = RegisterType(types, CLASS_Object);
    o->types = types;
}

Type
GetType(void const *object)
{
    return GetObject(object)->types->type[0];
}

Object
GetObject(void const *object)
{
    return (Object)object;
}

Object
GetObjectOf(void const *object, const Type T)
{
    int i;
    Object p = GetObject(object);
    TypeList types = p->types;

    while (types)
    {
	for (i = 0; i < types->n; ++i)
	{
	    if (types->type[i] == T) return p;
	}
	types = types->next;
    }
    return 0;
}

TypeList
CreateTypeList(void)
{
    TypeList types = XMALLOC(struct TypeList, 1);
    types->n = 0;
    types->next = 0;
    return types;
}

TypeList
RegisterType(TypeList types, const Type T)
{
    TypeList current;
    TypeList tmp;

    if (!types) types = CreateTypeList();

    current = types;

    while (current->n == 16) {
	if (!current->next) current->next = CreateTypeList();
	current = current->next;
    }
    current->type[current->n++] = T;

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

