/*
 * Dynamically loaded filter plugin for HDF5 blosc filter.
 * Copyright (C) 2016  The THG Group
 * Author: Francesc Alted
 * Date: 2016-09-27
 *
 */


#ifndef BLOSC_VERSION_H
#define BLOSC_VERSION_H

/* Version numbers */
#define BLOSC_PLUGIN_VERSION_MAJOR    1    /* for major interface/format changes  */
#define BLOSC_PLUGIN_VERSION_MINOR    0   /* for minor interface/format changes  */
#define BLOSC_PLUGIN_VERSION_INTERFACE  0    /*  */
#define BLOSC_PLUGIN_VERSION_RELEASE  0    /* for tweaks, bug-fixes, or development */

#define BLOSC_PLUGIN_VERSION_STRING   "1.0.0-dev"  /* string version.  Sync with above! */
#define BLOSC_PLUGIN_VERSION_DATE     "2016-09-03 #$"    /* date version */

#endif    // BLOSC_VERSION_H
