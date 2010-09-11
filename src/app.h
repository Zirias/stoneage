#ifndef STONEAGE_APP_H
#define STONEAGE_APP_H

#include "common.h"

/** @file app.h
 * Includes definition of class App.
 */

/** A class representing an application.
 * This is the common base class for Applications.
 */
CLASS(App)
{
    INHERIT(Object);

    /** run the application.
     * @param argc number of command line arguments
     * @param argv pointer to the command line arguments
     * @return exit code
     */
    int FUNC(run)(THIS, int argc, char **argv);

    /** abort the application
     * This function could be called from anywhere through the global
     * mainApp pointer. A derived class should make sure to properly
     * clean up and then exit with a negative return value.
     */
    void FUNC(abort)(THIS);
};

/** the global application pointer.
 * This should be set by derived classes in their constructor.
 */
extern App mainApp;

#endif
