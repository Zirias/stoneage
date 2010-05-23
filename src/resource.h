#ifndef STONEAGE_RESOURCE_H
#define STONEAGE_RESOURCE_H

#include <stdio.h>

#include "common.h"

struct Resource_impl;

/** @file resource.h
 * Includes definition of the Resource class
 */

/** Represents a single Resource.
 * A Resource object holds a block of binary data, identified by a string
 * key.
 */
CLASS(Resource)
{
    INHERIT(Object);

    struct Resource_impl *pimpl; /**< @private */

    /** Set the Resource's name.
     * Set's the string key for the resource. The maximum lenght is 255
     * characters, longer names will be silently cut off. The name data is
     * actually copied.
     * @param name the name for the Resource
     */
    void FUNC(setName)(THIS, const char *name);

    /** Get the Resource's name.
     * @return a pointer to the name of the Resource
     */
    const char *FUNC(getName)(THIS);

    /** Give a block of data to the Resource
     * Sets the Resource's data to the given block. No copying is done,
     * the Resource object takes ownership of the data.
     * @param data pointer to the new data
     * @param dataSize size of the new data in bytes
     * @return nonzero if the Resource already contained data
     */
    int FUNC(giveData)(THIS, void *data, size_t dataSize);

    /** Set data by copying it.
     * Sets the Resource's data by copying bytes from a given location.
     * @param data the source for the copy
     * @param dataSize the number of bytes to copy
     * @return nonzero if the Resource already contained data
     */
    int FUNC(copyDataFrom)(THIS, const void *data, size_t dataSize);

    /** Set data by reading from a file stream.
     * Sets the Resource's data by reading bytes from an opened file stream
     * @param file the file stream to read from
     * @param dataSize the number of bytes to read
     * @return nonzero on read error or if the Resource already contained data
     */
    int FUNC(readDataFrom)(THIS, FILE *file, size_t dataSize);

    /** Get the size of the Resource data.
     * @return the number of bytes of the Resource's data
     */
    size_t FUNC(getDataSize)(THIS);

    /** Get the Resource's data.
     * @return a pointer to the Resource's data
     */
    const void *FUNC(getData)(THIS);

    /** Take ownership of the Resource's data.
     * Returns a pointer to the Resource's data and makes the Resource object
     * forget about it.
     * @return a pointer to the data, must be released after use
     */
    void *FUNC(takeData)(THIS);

    /** Delete the Resource's data
     */
    void FUNC(deleteData)(THIS);
};

#endif
