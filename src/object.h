#ifndef STONEAGE_OBJECT_H
#define STONEAGE_OBJECT_H

#include "common.h"

/** @file object.h
 * Contains the declaration of the base Object class and a set of
 * macros for some common object oriented concepts
 */

/** Declare a constructor.
 * @relates Object
 */
#define CTOR(myclass) myclass myclass##_ctor(myclass this, TypeList types)

/** Declare a destructor.
 * @relates Object
 */
#define DTOR(myclass) void myclass##_dtor(myclass this)


/** Declare a class.
 * @relates Object
 */
#define CLASS(name) struct name; \
    typedef struct name *name; \
    extern CTOR(name); \
    extern DTOR(name); \
    struct name

/** Declare base class, should come first in class declaration.
 * @relates Object
 */
#define INHERIT(baseclass) struct baseclass _baseobject

/** Secure cast operator, returns NULL if pointer is not of given type.
 * @relates Object
 */
#define CAST(pointer, type) ((type)GetObjectOf(pointer, CLASS_##type))

/** Create a new object.
 * @relates Object
 */
#define NEW(T) T##_ctor((T)XMALLOC(struct T, 1), 0)

/** Delete an object.
 * @relates Object
 */
#define DELETE(T, object) do { if (object) { \
	T##_dtor((T)object); free(object); object=0; \
    }} while (0)

/** Call base constructor, mandatory for every constructor.
 * @relates Object
 */
#define BASECTOR(myclass, baseclass) \
    types = RegisterType(types, CLASS_##myclass); \
    baseclass##_ctor((baseclass)this, types)

/** Call base destructor, mandatory for every destructor.
 * @relates Object
 */
#define BASEDTOR(baseclass) baseclass##_dtor((baseclass)this)

/** Declare a method.
 * @relates Object
 */
#define FUNC(name) (* name )

/** starting boilerplate for method signature
 * @relates Object
 */
#define THIS void *_this

/** starting boilerplate for method implementation.
 * @relates Object
 */
#define METHOD(myclass) myclass this = CAST(_this, myclass)


struct TypeList;
typedef struct TypeList *TypeList;

/** The basic Object class.
 * every class must inherit from this one.
 */
CLASS(Object)
{
    /** A list of all possible types of the object */
    TypeList types;
};

/** Get the most derived type of the object.
 * @relates Object
 * @param object the object to check
 * @return the type
 */
extern Type GetType(void const *object);

/** Cast pointer to an object type.
 * @relates Object
 * @param object pointer to the object
 * @return the object as an Object instance
 */
extern Object GetObject(void const *object);

/** Cast pointer to an object type only if it matches a given type.
 * @relates Object
 * @param object pointer to the object
 * @param T the type to check for
 * @return the object as an Object instance
 */
extern Object GetObjectOf(void const *object, const Type T);

/** Add a type to a list of types.
 * @relates Object
 * This is used in constructors to iteratively create the complete type list
 * of the object
 * @param types the type list
 * @param T the type to add
 * @return the modified type list
 */
extern TypeList RegisterType(TypeList types, const Type T);

#endif
