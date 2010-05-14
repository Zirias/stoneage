#include <string.h>

#include "resource.h"

struct Resource_impl
{
    char* name;
    void* data;
    size_t dataSize;
};

void
m_setName ARG(const char *name)
{
    METHOD(Resource);

    this->pimpl->name = XREALLOC(char, this->pimpl->name, strlen(name) + 1);
    strcpy(this->pimpl->name, name);
}

const char *
m_getName ARG()
{
    METHOD(Resource);

    return this->pimpl->name;
}

int
m_giveData ARG(void *data, size_t dataSize)
{
    METHOD(Resource);

    if (this->pimpl->data) return -1;
    this->pimpl->data = data;
    this->pimpl->dataSize = dataSize;
    return 0;
}

int
m_copyDataFrom ARG(const void *data, size_t dataSize)
{
    METHOD(Resource);

    if (this->pimpl->data) return -1;
    this->pimpl->data = XMALLOC(void, dataSize);
    memcpy(this->pimpl->data, data, dataSize);
    this->pimpl->dataSize = dataSize;
    return 0;
}

int
m_readDataFrom ARG(FILE *file, size_t dataSize)
{
    METHOD(Resource);

    if(this->pimpl->data) return -1;
    this->pimpl->data = XMALLOC(void, dataSize);
    fread(this->pimpl->data, dataSize, 1, file);
    this->pimpl->dataSize = dataSize;
    return 0;
}

size_t
m_getDataSize ARG()
{
    METHOD(Resource);

    return this->pimpl->dataSize;
}

const void *
m_getData ARG()
{
    METHOD(Resource);

    return this->pimpl->data;
}

void *
m_takeData ARG()
{
    METHOD(Resource);

    void *data = this->pimpl->data;
    this->pimpl->data = 0;
    return data;
}

void
m_deleteData ARG()
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

