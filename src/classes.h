#ifndef STONEAGE_CLASSES_H
#define STONEAGE_CLASSES_H

/** @file classes.h
 * Contains CLASS type registry
 */

/** Enumeration of all CLASS types
 * Every class must be registered here, this info is needed for
 * "RTTI" and secure CAST() functionality
 */
enum Type {
    CLASS_Event,
    CLASS_EHandler,
    CLASS_Board,
    CLASS_EEarth,
    CLASS_EWall,
    CLASS_ERock,
    CLASS_ECabbage,
    CLASS_EWilly,
    CLASS_SaMkres,
    CLASS_Resource,
    CLASS_Resfile,
    CLASS_Stoneage,
    CLASS_Entity,
    CLASS_App,
    CLASS_Object
};
typedef enum Type Type;

#endif
