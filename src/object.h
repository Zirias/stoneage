#ifndef STONEAGE_OBJECT_H
#define STONEAGE_OBJECT_H

#include "common.h"

struct TypeList;
typedef struct TypeList *TypeList;

#define CLASS(name) struct name; \
    typedef struct name *name; \
    struct name

CLASS(Object) {
    TypeList types;
};

#define INHERIT(baseclass) struct baseclass _baseobject
#define CAST(pointer, type) ((type)GetObjectOf(pointer, CLASS_##type))
#define NEW(T) T##_ctor((T)XMALLOC(struct T, 1), 0)
#define DELETE(T, object) T##_dtor(object); XFREE(object)

#define BASECTOR(myclass, baseclass) \
    types = RegisterType(types, CLASS_##myclass); \
    baseclass##_ctor((baseclass)this, types)
#define BASEDTOR(baseclass) baseclass##_dtor((baseclass)this)

#define FUNC(name) (* name )
#define ARG(...) (void *_this, ##__VA_ARGS__)

#define METHOD(myclass) myclass this = CAST(_this, myclass)

#define CTOR(myclass) myclass myclass##_ctor(myclass this, TypeList types)
#define DTOR(myclass) void myclass##_dtor(myclass this)

extern CTOR(Object);
extern DTOR(Object);

extern Type GetType(void const *object);
extern Object GetObject(void const *object);
extern Object GetObjectOf(void const *object, const Type T);
extern TypeList RegisterType(TypeList types, const Type T);

#endif
