/*
    FuseCompress
    Copyright (C) 2005 Milan Svoboda <milan.svoboda@centrum.cz>
*/

#include <pthread.h>
#include <sys/types.h>
#include <assert.h>

#include "structs.h"
#include "compress.h"

pthread_t           pt_comp;	/* compress thread */
pthread_mutexattr_t locktype;

// Files smaller than this are not compressed
//
int min_filesize_background;

// Files smaller than this are not direct compressed
//
int min_filesize_direct;

compressor_t *compressor_default = NULL;

// Table of supported compressors. This is array and
// it is vital to sort modules according it's type. E.g. module_null
// has type 0x0 or module_gzip has type 0x02.
//
compressor_t *compressors[4] = {
	&module_null,
	&module_bz2,
	&module_gzip,
	&module_lzo,
};

char *uncompressible[] = {
    ".mp3", ".ogg",
    ".avi", ".mov", 
    ".gz", ".bz2", ".zip", ".tgz", ".lzo",
    ".jpg", ".png", ".tiff", ".gif",
    ".rpm", ".deb",
    NULL
};

database_t database = {
	.head = LIST_HEAD_INIT(database.head),
	.lock = PTHREAD_MUTEX_INITIALIZER,
	.cond = PTHREAD_COND_INITIALIZER,		// Not used
	.entries = 0,
};

database_t comp_database = {
	.head = LIST_HEAD_INIT(comp_database.head),
	.lock = PTHREAD_MUTEX_INITIALIZER,
	.cond = PTHREAD_COND_INITIALIZER,		// When new item is added to the list
	.entries = 0,
};