/*
* Copyright (c) 2011 Nathanael Hübbe
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <time.h>
#include <hdf5.h>

#include "mafisc.h"

#define allocate(type, name, count) type* name = (type*)malloc(sizeof(type)*count)

const hsize_t dims[3] = { 200, 250, 300 };
const hsize_t chunk[3] = { 50, 50, 50 };
const char* const testFileName = "compressed-data.h5";
const char* const datasetName = "faked and compressed data";

int filterTest(void) {
	herr_t result;
	hid_t file, space, dcpl, dset;
	//Init
	srand(42);
	//Create a file with a compressed dataset.
    file = H5Fcreate(testFileName, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    space = H5Screate_simple(3, dims, NULL);
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    if((result = H5Pset_chunk(dcpl, 3, chunk))) return result;
	if((result = H5Pset_filter(dcpl, MAFISC_ID, H5Z_FLAG_OPTIONAL, 0, NULL))) return result;
    dset = H5Dcreate2(file, datasetName, H5T_STD_I32LE, space, H5P_DEFAULT, dcpl, H5P_DEFAULT);
	{
		//Fake some data.
		size_t valueCount = dims[0]*dims[1]*dims[2], i;
		allocate(double, data, valueCount);
		clock_t start;
		double seconds;
		printf("faking data...\n");
		for(i = 0; i < valueCount; i++) data[i] = (double)i;
		//Write it to our awesome dataset.
		printf("writing...\n");
		start = clock();
		if((result = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data))) return result;
		seconds = ((double)(clock() - start))/CLOCKS_PER_SEC;
		printf("\ttook %g seconds, %g MB/s.\n", seconds, valueCount*sizeof(double)/(1000000*seconds));
		//Tidy up.
		if((result = H5Pclose(dcpl))) return result;
		if((result = H5Dclose(dset))) return result;
		if((result = H5Sclose(space))) return result;
		if((result = H5Fclose(file))) return result;
		{
			char* commandLine;
			if(asprintf(&commandLine, "ls -l %s", testFileName) < 0) return -1;
			int trash = system(commandLine);
			if(trash & ~trash) abort();
			free(commandLine);
		}
		//Clobber the fake data
		for(i = 0; i < valueCount; i++) data[i] = (double)0;
		//Reopen the file with the compressed dataset.
		printf("reading...\n");
		file = H5Fopen(testFileName, H5F_ACC_RDONLY, H5P_DEFAULT);
		dset = H5Dopen2(file, datasetName, H5P_DEFAULT);
		start = clock();
		if((result = H5Dread(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data))) return result;
		seconds = ((double)(clock() - start))/CLOCKS_PER_SEC;
		printf("\ttook %g seconds, %g MB/s.\n", seconds, valueCount*sizeof(double)/(1000000*seconds));
//		printDkrzFilterTimes();
		//Check the data
		for(i = 0; i < valueCount; i++) {
			if(data[i] != (double)i) {	//The compiler complains about unsave float comparison, but here I want to be sure that the values are identical to the last bit.
				fprintf(stderr, "Error: Value %li == %g\n", i, data[i]);
				break;
			}
		}
		//Tidy up.
		if((result = H5Dclose(dset))) return result;
		if((result = H5Fclose(file))) return result;
		free(data);
		return 0;
	}
}
