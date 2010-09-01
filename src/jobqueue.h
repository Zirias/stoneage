#ifndef STONEAGE_JOBQUEUE_H
#define STONEAGE_JOBQUEUE_H

#include "common.h"

/** @file jobqueue.h
 * includes definition of class JobQueue
 */

struct JobQueue_impl;

typedef void (* Job )(void *data);

/** A class for enqueuing jobs to be executed later.
 * This class implements a queue for jobs. It is used to provide jobs
 * for the main loop to execute between the processing of events
 */
CLASS(JobQueue)
{
    INHERIT(Object);

    struct JobQueue_impl *pimpl; /**< private */

    /** Initialize the queue.
     * Initializes the job queue, must be called before enqueueing jobs
     * @param queueSize size of the queue
     */
    void FUNC(init)(THIS, int queueSize);

    /** Enqueue a new job.
     * Enqueues a new job to be executed later
     * @param job a pointer to the function to call
     * @param data data to pass to the function
     * @return 0 on success, -1 on error (queue full or not initialized)
     */
    int FUNC(enqueue)(THIS, Job job, void *data);

    /** Executes the next job.
     * Executes the next job and removes it from the queue
     * @return 0 on success, -1 on error (queue empty or not initialized)
     */
    int FUNC(callNext)(THIS);
};

#endif
