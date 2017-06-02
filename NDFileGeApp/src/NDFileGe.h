/*
 * NDFileTIFF.h
 * Writes NDArrays to TIFF files.
 * John Hammonds
 * April 17, 2009
 */

#ifndef NDFileGe_H
#define NDFileGe_H


#ifdef _WIN32
#include <windows.h>

#endif


#ifndef _WIN32
#define _stat stat
#define _chmod chmod

#endif


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
using std::string;
#ifdef _WIN32
#include <direct.h>
#endif




#include "NDPluginFile.h"
//#include "tiffio.h"

/* This version number is an attribute in the TIFF file to allow readers
 * to handle changes in the file contents */


/**
 * Binary format file writer for Ge Strip detector. Straight binary that assumes dims in ND array are just memory size.
 * Actual num  bytes written are in attrubutes. attr read then that num of bytes are written
  
  
  */

class NDFileGe : public NDPluginFile {
        public:
                NDFileGe(const char *portName,int max_ge_bytes, int queueSize, int blockingCallbacks,
                                const char *NDArrayPort, int NDArrayAddr,
                                int priority, int stackSize);

                virtual void processCallbacks(NDArray *pArray);
                /* The methods that this class implements */
                virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray);
                virtual asynStatus readFile(NDArray **pArray);
                virtual asynStatus writeFile(NDArray *pArray);
                virtual asynStatus closeFile();

                asynStatus writeOctet(
                                asynUser *pasynUser, 
                                const char *value,
                                size_t nChars, 
                                size_t *nActual);

        protected:
                int *pAttributeId;

                int max_ge_bytes;


                NDAttribute *pAttribute;
                char name[256];
                char description[256];
                char tempString[256];
                NDAttrDataType_t attrDataType;
                size_t attrSize;
                int numAttributes, attrCount;


                enum which_timestamp {
                        xcorecoticks,
                        xfpga_ts,
                        xarray_timestamp,
                        xarray_unique_id

                };

                FILE *myfile;

                NDArray *my_array;

                //
                // Params
                //
                
                int NDFileGe_num_events;
                int NDFileGe_fnum;
                int NDFileGe_printAttributes;
                enum {num_params=3};


                int getIntParam(int param){
                        int output;
                        getIntegerParam(param,&output);
                        return(output);
                };


                double getDoubParam(int param){
                        double output;
                        getDoubleParam(param,&output);
                        return(output);
                };

};

#endif
