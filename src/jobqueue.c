#include "jobqueue.h"

typedef struct
{
    Job job;
    void *data;
} JobQueueEntry;

struct JobQueue_impl
{
    JobQueueEntry *head;
    JobQueueEntry *tail;
    JobQueueEntry *last;
    int entries;

    JobQueueEntry *queue;
};

static void
m_init(THIS, int queueSize)
{
    METHOD(JobQueue);

    struct JobQueue_impl *j = this->pimpl;
    XFREE(j->queue);
    j->queue = XMALLOC(JobQueueEntry, queueSize);
    j->last = j->queue + queueSize;
    j->tail = j->last;
    j->head = j->last;
    j->entries = 0;
}

static int
m_enqueue(THIS, Job job, void *data)
{
    METHOD(JobQueue);

    JobQueueEntry *entry;

    struct JobQueue_impl *j = this->pimpl;
    if (!j->queue) return -1;
    if (j->head == j->tail && j->entries) return -1;
    entry = --(j->head);
    if (entry == j->queue) j->head = j->last;
    entry->job = job;
    entry->data = data;
    ++(j->entries);
    return 0;
}

static int
m_callNext(THIS)
{
    METHOD(JobQueue);

    JobQueueEntry *entry;

    struct JobQueue_impl *j = this->pimpl;
    if (!j->queue) return -1;
    if (!j->entries) return -1;
    entry = --(j->tail);
    if (entry == j->queue) j->tail = j->last;
    --(j->entries);
    entry->job(entry->data);
    return 0;
}

CTOR(JobQueue)
{
    BASECTOR(JobQueue, Object);
    this->pimpl = XMALLOC(struct JobQueue_impl, 1);
    this->pimpl->queue = 0;

    this->init = &m_init;
    this->enqueue = &m_enqueue;
    this->callNext = &m_callNext;

    return this;
}

DTOR(JobQueue)
{
    XFREE(this->pimpl->queue);
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

