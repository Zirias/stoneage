#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "resfile.h"
#include "app.h"

static const char *magic = "SARESV\0\1";
static struct stat statrec;

struct Resfile_entry;
typedef struct Resfile_entry *Resfile_entry;

struct Resfile_entry
{
    char *key;
    off_t offset;
    uint32_t size;
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

const char *openmode[3] = { "rb", "rb+", "wb+" };

static size_t
writeUint32(FILE *file, uint32_t val)
{
    uint8_t bytes[5];
    bytes[0] = 0;
    while (val)
    {
	++bytes[0];
	bytes[bytes[0]] = val & 0xff;
	val >>= 8;
    }
    size_t count = (size_t) bytes[0];
    fwrite(&bytes[0], 1, count + 1, file);
    return count;
}

static size_t
readUint32(FILE *file, uint32_t *val)
{
    uint8_t bytes[5];
    uint8_t i;

    fread(&bytes[0], 1, 1, file);
    fread(&bytes[1], (size_t) bytes[0], 1, file);
    *val = 0;
    for (i = bytes[0]; i > 0; --i)
    {
	*val <<= 8;
	*val += bytes[i];
    }
    return (size_t) bytes[0];
}

static void
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

static const Resfile_entry
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

static int
m_canRead ARG()
{
    METHOD(Resfile);

    return (this->pimpl->opened);
}

static int
m_canWrite ARG()
{
    METHOD(Resfile);

    return (this->pimpl->opened && this->pimpl->writeable);
}

static int
m_setFile ARG(const char *filename)
{
    METHOD(Resfile);

    if (this->pimpl->opened) return -1;
    XFREE(this->pimpl->filename);
    this->pimpl->filename = XMALLOC(char, strlen(filename) + 1);
    strcpy(this->pimpl->filename, filename);
    return 0;
}

static int
m_open ARG(int writeable)
{
    METHOD(Resfile);
    
    char checkMagic[9];
    uint8_t keysize;
    uint32_t ressize;
    off_t offset;
    char *key;
    int ferr;
    Resfile_toc toc;
    Resfile_entry entry;

    if (this->pimpl->opened) return -1;
    if (!this->pimpl->filename) return -1;

    if (writeable) writeable = 1;
    if (stat(this->pimpl->filename, &statrec) < 0)
    {
	if (errno == ENOENT && writeable)
	{
	    ++writeable;
	}
	else
	{
	    log_err("Error reading resource file %s: %s\n",
		    this->pimpl->filename, strerror(errno));
	    mainApp->abort(mainApp);
	}
    }
    else if (!S_ISREG(statrec.st_mode))
    {
	log_err("Resource file %s is not a regular file!\n",
		this->pimpl->filename);
	mainApp->abort(mainApp);
    }

    this->pimpl->file = fopen(this->pimpl->filename, openmode[writeable]);
    if (!this->pimpl->file) return -1;

    if (writeable == 2)
    {
	fwrite(magic, sizeof(char), 9, this->pimpl->file);
    }
    else
    {
	fread(checkMagic, sizeof(char), 9, this->pimpl->file);
	if (strncmp(checkMagic, magic, 6))
	{
	    fclose(this->pimpl->file);
	    log_err("%s is not a stoneage resource file!\n",
		    this->pimpl->filename);
	    mainApp->abort(mainApp);
	}
    }

    toc = &(this->pimpl->toc);
    offset = 9;
    while (fread(&keysize, sizeof(uint8_t), 1, this->pimpl->file))
    {
	++(this->pimpl->num_resources);
	key = XMALLOC(char, (size_t) keysize + 1);
	key[keysize] = 0;
	fread(key, keysize * sizeof(char), 1, this->pimpl->file);
	readUint32(this->pimpl->file, &ressize);
	if (toc->n == 128)
	{
	    toc->next = XMALLOC(struct Resfile_toc, 1);
	    toc = toc->next;
	    memset(toc, 0, sizeof(struct Resfile_toc));
	}
	entry = &(toc->entry[toc->n++]);
	entry->key = key;
	entry->offset = offset;
	entry->size = (size_t) ressize;
	fseek(this->pimpl->file, (long) ressize, SEEK_CUR);
	offset = ftell(this->pimpl->file);
    }
    if ((ferr = ferror(this->pimpl->file)) != 0)
    {	
	fclose(this->pimpl->file);
	log_err("Resource file %s is corrupt: %s\n",
		this->pimpl->filename, strerror(ferr));
	mainApp->abort(mainApp);
    }
    rewind(this->pimpl->file);

    this->pimpl->opened = 1;
    this->pimpl->writeable = writeable;
    return 0;
}

static void
m_close ARG()
{
    METHOD(Resfile);

    if (!this->pimpl->opened) return;
    fclose(this->pimpl->file);
    clearToc(&(this->pimpl->toc));
    this->pimpl->opened = 0;
}

static int
m_store ARG(Resource res)
{
    METHOD(Resfile);

    Resfile_toc toc;
    Resfile_entry entry;
    const char *key;
    uint8_t keysize;

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
    keysize = (uint8_t) strlen(key);
    entry->key = XMALLOC(char, (size_t) (keysize + 1));
    strcpy(entry->key, key);
    entry->size = res->getDataSize(res);
    fseek(this->pimpl->file, 0, SEEK_END);
    entry->offset = ftell(this->pimpl->file);
    fwrite(&keysize, sizeof(uint8_t), 1, this->pimpl->file);
    fwrite(entry->key, (size_t) keysize, 1, this->pimpl->file);
    writeUint32(this->pimpl->file, (uint32_t) entry->size);
    
    fwrite(res->getData(res), entry->size, 1, this->pimpl->file);
    rewind(this->pimpl->file);
    ++(this->pimpl->num_resources);
    return 0;
}

static int
m_load ARG(const char *key, Resource *res)
{
    METHOD(Resfile);

    Resfile_entry entry;
    uint32_t size;

    if (!this->pimpl->opened) return -1;

    entry = findEntry(&(this->pimpl->toc), key);
    if (!entry) return -1;

    Resource r = NEW(Resource);
    r->setName(r,key);
    fseek(this->pimpl->file, entry->offset + sizeof(uint8_t)
	    + strlen(key), SEEK_SET);
    readUint32(this->pimpl->file, &size);
    if ((size_t) size != entry->size)
    {
	log_err("fatal: corrupted resource file!");
	this->close(this);
	mainApp->abort(mainApp);
    }
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

