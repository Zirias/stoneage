#ifndef STONEAGE_OBJECT_H
#define STONEAGE_OBJECT_H

#include "common.h"

struct TypeList;
typedef struct TypeList *TypeList;

struct Object {
    TypeList types;
};

typedef struct Object *Object;

#define CLASS() struct Object _baseobject
#define INHERIT(baseclass) struct baseclass _baseobject
#define CAST(pointer, type) ((type)GetObjectOf(pointer, CLASS_##type))
#define NEW(T) T##_ctor((T)XMALLOC(struct T, 1), 0)
#define DELETE(T, object) T##_dtor(object); XFREE(object)

#define INHERIT_INIT(pointer, myclass, baseclass) \
    types = RegisterType(types, CLASS_##myclass); \
    baseclass##_ctor((baseclass)pointer, types)
#define OBJECT_INIT(pointer, myclass) INHERIT_INIT(pointer, myclass, Object)

extern Object Object_ctor(Object o, TypeList types);
extern void Object_dtor(Object o);
extern Type GetType(void const *object);
extern Object GetObject(void const *object);
extern Object GetObjectOf(void const *object, const Type T);
extern TypeList RegisterType(TypeList types, const Type T);

#endif
