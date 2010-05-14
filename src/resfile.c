#include <string.h>

#include "resfile.h"

struct Resfile_entry;
typedef struct Resfile_entry *Resfile_entry;

struct Resfile_entry
{
    char *key;
    off_t offset;
    size_t size;
};

struct Resfile_toc;
typedef struct Resfile_toc *Resfile_toc;

struct Resfile_toc
{
    int n;
    struct Resfile_entry entry[128];
    Resfile_toc next;
};

struct Resfile_impl
{
    int opened;
    int writeable;
    char *filename;
    int num_resources;
    struct Resfile_toc toc;
    FILE *file;
};

const char *openmode[2] = { "rb", "ab+" };

void
clearToc(Resfile_toc toc)
{
    int i;

    if (toc->next)
    {
	clearToc(toc->next);
	XFREE(toc->next);
    }
    for (i = 0; i < toc->n; ++i) XFREE(toc->entry[i].key);
    toc->n = 0;
}

const Resfile_entry
findEntry(Resfile_toc toc, const char *key)
{
    int i;

    for (i = 0; i < toc->n; ++i)
    {
	if (!(strcmp(toc->entry[i].key, key))) return &(toc->entry[i]);
    }
    if (!toc->next) return 0;
    return findEntry(toc->next, key);
}

int
m_canRead ARG()
{
    METHOD(Resfile);

    return (this->pimpl->opened);
}

int
m_canWrite ARG()
{
    METHOD(Resfile);

    return (this->pimpl->opened && this->pimpl->writeable);
}

int
m_setFile ARG(const char *filename)
{
    METHOD(Resfile);

    if (this->pimpl->opened) return -1;
    XFREE(this->pimpl->filename);
    this->pimpl->filename = XMALLOC(char, strlen(filename) + 1);
    strcpy(this->pimpl->filename, filename);
    return 0;
}

int
m_open ARG(int writeable)
{
    METHOD(Resfile);
    
    size_t keysize;
    size_t ressize;
    off_t offset;
    char *key;
    Resfile_toc toc;
    Resfile_entry entry;

    if (this->pimpl->opened) return -1;
    if (!this->pimpl->filename) return -1;

    this->pimpl->file = fopen(this->pimpl->filename, openmode[writeable]);
    if (!this->pimpl->file) return -1;
    rewind(this->pimpl->file);

    toc = &(this->pimpl->toc);
    offset = 0;
    while (fread(&keysize, sizeof(size_t), 1, this->pimpl->file))
    {
	++(this->pimpl->num_resources);
	key = XMALLOC(char, keysize + 1);
	key[keysize] = 0;
	fread(key, keysize * sizeof(char), 1, this->pimpl->file);
	fread(&ressize, sizeof(size_t), 1, this->pimpl->file);
	if (toc->n == 128)
	{
	    toc->next = XMALLOC(struct Resfile_toc, 1);
	    toc = toc->next;
	    memset(toc, 0, sizeof(struct Resfile_toc));
	}
	entry = &(toc->entry[toc->n++]);
	entry->key = key;
	entry->offset = offset;
	entry->size = ressize;
	fseek(this->pimpl->file, ressize, SEEK_CUR);
	offset = ftell(this->pimpl->file);
    }
    rewind(this->pimpl->file);

    this->pimpl->opened = 1;
    this->pimpl->writeable = writeable;
    return 0;
}

void
m_close ARG()
{
    METHOD(Resfile);

    if (!this->pimpl->opened) return;
    fclose(this->pimpl->file);
    clearToc(&(this->pimpl->toc));
    this->pimpl->opened = 0;
}

int
m_store ARG(Resource res)
{
    METHOD(Resfile);

    Resfile_toc toc;
    Resfile_entry entry;
    const char *key;
    size_t keysize;

    if (!this->pimpl->opened) return -1;
    if (!this->pimpl->writeable) return -1;

    toc = &(this->pimpl->toc);
    key = res->getName(res);
    if (findEntry(toc, key)) return -1;

    while (toc->next) toc = toc->next;
    if (toc->n == 128)
    {
	toc->next = XMALLOC(struct Resfile_toc, 1);
	toc = toc->next;
	memset(toc, 0, sizeof(struct Resfile_toc));
    }
    entry = &(toc->entry[toc->n++]);
    keysize = strlen(key);
    entry->key = XMALLOC(char, keysize + 1);
    strcpy(entry->key, key);
    entry->size = res->getDataSize(res);
    fseek(this->pimpl->file, 0, SEEK_END);
    entry->offset = ftell(this->pimpl->file);
    fwrite(&keysize, sizeof(size_t), 1, this->pimpl->file);
    fwrite(entry->key, keysize, 1, this->pimpl->file);
    fwrite(&(entry->size), sizeof(size_t), 1, this->pimpl->file);
    fwrite(res->getData(res), entry->size, 1, this->pimpl->file);
    rewind(this->pimpl->file);
    ++(this->pimpl->num_resources);
    return 0;
}

int
m_load ARG(const char *key, Resource *res)
{
    METHOD(Resfile);

    Resfile_entry entry;

    if (!this->pimpl->opened) return -1;

    entry = findEntry(&(this->pimpl->toc), key);
    if (!entry) return -1;

    Resource r = NEW(Resource);
    r->setName(r,key);
    fseek(this->pimpl->file, entry->offset
	    + 2*sizeof(size_t) + strlen(key), SEEK_SET);
    r->readDataFrom(r, this->pimpl->file, entry->size);
    *res = r;
    return 0;
}

CTOR(Resfile)
{
    BASECTOR(Resfile, Object);
    this->pimpl = XMALLOC(struct Resfile_impl, 1);
    memset(this->pimpl, 0, sizeof(struct Resfile_impl));
    this->canRead = &m_canRead;
    this->canWrite = &m_canWrite;
    this->setFile = &m_setFile;
    this->open = &m_open;
    this->close = &m_close;
    this->store = &m_store;
    this->load = &m_load;
    return this;
}

DTOR(Resfile)
{
    this->close(this);
    XFREE(this->pimpl->filename);
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

