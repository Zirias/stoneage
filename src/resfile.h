#ifndef STONEAGE_RESFILE_H
#define STONEAGE_RESFILE_H

#include <stdio.h>

#include "common.h"

struct Resource;

struct Resfile_impl;

/** @file resfile.h
 * Includes definition of the Resfile class
 */

/** Represents a resource file.
 * This class provides methods for storing blocks of binary data in a
 * resource file and reading them back. Each block is identified by a
 * string key.
 */
CLASS(Resfile)
{
    INHERIT(Object);

    struct Resfile_impl *pimpl; /**< @private */

    /** Check whether reading is possible.
     * @return 1 if file is opened and readable, 0 otherwise
     */
    int FUNC(canRead)(THIS);

    /** Check whether writing is possible.
     * @return 1 if file is opened and writeable, 0 otherwise
     */
    int FUNC(canWrite)(THIS);

    /** Set the filename.
     * Sets the resource file's name for a subsequent call to open.
     * Not allowed when the file is already opened.
     * @param filename the file name
     * @return non-zero if the file is already opened
     */
    int FUNC(setFile)(THIS, const char *filename);

    /** Open the resource file.
     * This method opens and scans the resource file, creating a table
     * of available keys. Call setFile() first to set a filename.
     * @param writeable flag for opening the file in writeable mode,
     *   possibly creating it first
     * @return non-zero on error
     */
    int FUNC(open)(THIS, int writeable);

    /** Close the resource file.
     * This method closes the resource file and releases the table of
     * available keys.
     */
    void FUNC(close)(THIS);

    /** Store a resource to the file.
     * This method tries to store a given resource to the file. It will fail
     * if either the file is not opened and writeable or if it already
     * contains a Resource with the same key as the given one.
     * @param res the Resource to store.
     * @return non-zero on error
     */
    int FUNC(store)(THIS, struct Resource *res);

    /** Load a resource from the file.
     * This method loads a Resource from the resource file. It will fail if
     * either the file is not opened and readable or if the resource is
     * not found in the file.
     * @param key the key of the Resource.
     * @param res a pointer to a Resource reference that is assigned a newly
     *   created Resource object.
     * @return non-zero on error
     */
    int FUNC(load)(THIS, const char *key, struct Resource **res);
};

#endif
