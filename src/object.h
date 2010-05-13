#ifndef STONEAGE_OBJECT_H
#define STONEAGE_OBJECT_H

#include "common.h"

enum ClassType {
    CEntity,
    CObject
};

struct TypeList;
typedef struct TypeList *TypeList;

struct Object {
    TypeList types;
};

typedef struct Object *Object;

#define CLASS() struct Object _object
#define INHERIT(baseclass) struct baseclass _object
#define CAST(pointer, type) ((type)GetObjectOf(pointer, C##type))
#define NEW(T) T##_ctor((T)XMALLOC(struct T, 1), 0)
#define DELETE(T, object) T##_dtor(object); XFREE(object)

#define INHERIT_INIT(pointer, myclass, baseclass) \
    types = RegisterType(types, C##myclass); \
    baseclass##_ctor((baseclass)pointer, types)
#define OBJECT_INIT(pointer, myclass) INHERIT_INIT(pointer, myclass, Object)

extern Object Object_ctor(Object o, TypeList types);
extern void Object_dtor(Object o);
extern enum ClassType GetType(void const *object);
extern Object GetObject(void const *object);
extern Object GetObjectOf(void const *object, const enum ClassType T);
extern TypeList RegisterType(TypeList types, const enum ClassType T);

#endif
