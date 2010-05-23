#include <string.h>

#include "resource.h"

struct Resource_impl
{
    char* name;
    void* data;
    size_t dataSize;
};

static void
m_setName(THIS, const char *name)
{
    METHOD(Resource);

    size_t namelen = strlen(name) + 1;
    if (namelen > 256) namelen = 256;
    this->pimpl->name = XREALLOC(char, this->pimpl->name, namelen);
    strncpy(this->pimpl->name, name, namelen);
}

static const char *
m_getName(THIS)
{
    METHOD(Resource);

    return this->pimpl->name;
}

static int
m_giveData(THIS, void *data, size_t dataSize)
{
    METHOD(Resource);

    if (this->pimpl->data) return -1;
    this->pimpl->data = data;
    this->pimpl->dataSize = dataSize;
    return 0;
}

static int
m_copyDataFrom(THIS, const void *data, size_t dataSize)
{
    METHOD(Resource);

    if (this->pimpl->data) return -1;
    this->pimpl->data = xmalloc(dataSize);
    memcpy(this->pimpl->data, data, dataSize);
    this->pimpl->dataSize = dataSize;
    return 0;
}

static int
m_readDataFrom(THIS, FILE *file, size_t dataSize)
{
    METHOD(Resource);

    if(this->pimpl->data) return -1;
    this->pimpl->data = xmalloc(dataSize);
    fread(this->pimpl->data, dataSize, 1, file);
    this->pimpl->dataSize = dataSize;
    return 0;
}

static size_t
m_getDataSize(THIS)
{
    METHOD(Resource);

    return this->pimpl->dataSize;
}

static const void *
m_getData(THIS)
{
    METHOD(Resource);

    return this->pimpl->data;
}

static void *
m_takeData(THIS)
{
    METHOD(Resource);

    void *data = this->pimpl->data;
    this->pimpl->data = 0;
    return data;
}

static void
m_deleteData(THIS)
{
    METHOD(Resource);

    XFREE(this->pimpl->data);
}

CTOR(Resource)
{
    BASECTOR(Resource, Object);
    this->pimpl = XMALLOC(struct Resource_impl, 1);
    this->pimpl->name = 0;
    this->pimpl->data = 0;
    this->setName = &m_setName;
    this->getName = &m_getName;
    this->giveData = &m_giveData;
    this->copyDataFrom = &m_copyDataFrom;
    this->readDataFrom = &m_readDataFrom;
    this->getDataSize = &m_getDataSize;
    this->getData = &m_getData;
    this->takeData = &m_takeData;
    this->deleteData = &m_deleteData;
    return this;
}

DTOR(Resource)
{
    XFREE(this->pimpl->name);
    XFREE(this->pimpl->data);
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

