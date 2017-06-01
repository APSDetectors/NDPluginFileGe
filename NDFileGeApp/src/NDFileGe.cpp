

/* NDFileGe.cpp
 * Writes NDArrays to Ge files.
 *
 * Timothy Madden
 * April 17, 2008
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netcdf.h>

#include <epicsStdio.h>
#include <epicsExport.h>
#include <iocsh.h>
#include <math.h>
#include "NDPluginFile.h"
#include "NDFileGe.h"
#include "drvNDFileGe.h"
// #include "tiffio.h"
#include "asynNDArrayDriver.h"
#include <ADCoreVersion.h>

#ifdef _WIN32
#include <io.h>
#endif
static const char *driverName = "NDFileGe";




/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERR(e) {asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, \
                "%s:%s error=%s\n", \
                driverName, functionName, nc_strerror(e)); \
                return(asynError);}

/* NDArray string attributes can be of any length, but netCDF requires a fixed maximum length
 * which we define here. */
#define MAX_ATTRIBUTE_STRING_SIZE 256

/** This is called to open a TIFF file.
*/
asynStatus NDFileGe::openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray)
{
    static const char *functionName = "openFile";

    /* We don't support reading yet */
    if (openMode & NDFileModeRead) return(asynError);

    /* We don't support opening an existing file for appending yet */
    if (openMode & NDFileModeAppend) return(asynError);


	myfile = fopen(fileName,"wb");
	
	if (myfile==0)
		return(asynError);

    return(asynSuccess);
}

/** This is called to write data a single NDArray to the file.  It can be called multiple times
 *  to add arrays to a single file in stream or capture mode */
asynStatus NDFileGe::writeFile(NDArray *pArray)
{
	unsigned long int stripsize;
    static const char *functionName = "writeFile";
	int sizex, sizey;
	int is_update;
	int fnx;
	int is_throw_frame=0;
	size_t dimSizeOut[ND_ARRAY_MAX_DIMS];
    int i;








        int ge_bytes = cf->getNumBytes(geh);
        
        if (cf->myfile != 0)
            cf->myfile->write((char*)(pArray->pData),ge_bytes);
    
        this->nextRecord++;

        getIntegerParam(NDFileNumber,&fnx);

  
	setIntegerParam(NDFileNumCaptured,this->nextRecord);
    getIntegerParam(NDFileNumCapture,&ii0);
    
    // if num to captuer is -1 we wioll run forever.
	if ((this->nextRecord >= ii0) && (ii0 != -1))
	{
		setIntegerParam(NDFileNumCaptured,ii0);
		closeFile();
		setIntegerParam(NDFileCapture, 0);
	}

    callParamCallbacks();
	return(asynSuccess);
}
/*******************************************************************************************
 *
 *
 *
 ******************************************************************************************/

void NDFileGe::processCallbacks(NDArray *pArray)
{
	bool is_at_msg;
	double fr_prd;
	/*

	,
	
	,
	,

	*/

	//getAttributes(pArray);

	is_at_msg=false;
	threshold = 0;
	is_ge_comp = 0;
	acq_time = 0.0;

    is_already_ge=0;
    pipe_num_shorts=0;

// dbpf "SimMaddog:Ge:EnableCallbacks","Enable"



	
	numAttributes = pArray->pAttributeList->count();

//	printf("Num Attributes %i \n", numAttributes);
	pAttribute = pArray->pAttributeList->next(NULL);


	if (getIntParam(NDFileGe_printAttributes))
		is_at_msg=true;

	for (attrCount=0; attrCount<numAttributes; attrCount++)
	{

        pAttribute->getValueInfo(&attrDataType, &attrSize);
        strcpy(name, pAttribute->getName());
        strcpy(description, pAttribute->getDescription());
                // pAttribute->getDescription(description, sizeof(description));
		//pAttribute->getValue(attrDataType, void *pValue, attrSize);

		// ge_threshold
		// ge_acq_time
		// ge_coreco_ticks

		if (is_at_msg)
		{
			printf("Attr: %s \n",name);


		}



		if (strcmp(name,"ge_threshold")==0)
		{
			pAttribute->getValue(attrDataType, (void*)&threshold, attrSize);
			if (is_at_msg)
				printf("ge_threshold = %i\n", threshold);
		}

	

        if (strcmp(name,"is_already_ge")==0)
        {
            pAttribute->getValue(attrDataType, (void*)&is_already_ge, attrSize);
            if (is_at_msg)
                printf("is_already_ge = %d\n", is_already_ge);
        }
     


	        pAttribute = pArray->pAttributeList->next(pAttribute);
		// pAttribute = pArray->nextAttribute(pAttribute);

	}

	setIntegerParam(NDFileGe_is_ge_comp,is_ge_comp);
        setIntegerParam(NDFileGe_threshold,threshold);
	


	ge_pixels=0;


	// call base class function...
	NDPluginFile::processCallbacks(pArray);

	setIntegerParam(NDFileGe_num_ge_pixels,ge_pixels);
	

    callParamCallbacks();
}

asynStatus NDFileGe::readFile(NDArray **pArray)
{
    //static const char *functionName = "readFile";

    return asynError;
}


asynStatus NDFileGe::closeFile()
{
    static const char *functionName = "closeFile";
	int fnx;
	int is_update;

    printf("Ge closeFile\n");
		cf->close();


			return asynSuccess;
}





/** Called when asyn clients call pasynOctet->write().
 * Catch parameter changes.  If the user changes the path or name of the template file
 * load the new template file.
 * \param[in] pasynUser pasynUser structure that encodes the reason and address.
 * \param[in] value Address of the string to write.
 * \param[in] nChars Number of characters to write.
 * \param[out] nActual Number of characters actually written. */
asynStatus NDFileGe::writeOctet(asynUser *pasynUser, const char *value,
      size_t nChars, size_t *nActual)
{
   int addr = 0;
   int function = pasynUser->reason;
   asynStatus status = asynSuccess;
   const char *functionName = "writeOctet";
char pathstr[512];

   status = getAddress(pasynUser, &addr);
   if (status != asynSuccess)
      return (status);
   /* Set the parameter in the parameter library. */
   status = (asynStatus) setStringParam(addr, function, (char *) value);

   char mesx[256];
   char *mesx2;
   
   getParamName(function, (const char**)&mesx2);
  
  
      /* If this parameter belongs to a base class call its method */
      status = NDPluginFile::writeOctet(pasynUser, value, nChars, nActual);
   

   /* Do callbacks so higher layers see any changes */
   status = (asynStatus) callParamCallbacks(addr, addr);

   if (status)
      epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
            "%s:%s: status=%d, function=%d, value=%s", driverName, functionName,
            status, function, value);
   else
      asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
            "%s:%s: function=%d, value=%s\n", driverName, functionName,
            function, value);
   *nActual = nChars;
   return status;
}






/* Configuration routine.  Called directly, or from the iocsh function in drvNDFileEpics */

extern "C" int drvNDFileGeConfigure(const char *portName, int max_ge_bytes, int queueSize, int blockingCallbacks,
                                   const char *NDArrayPort, int NDArrayAddr,
                                   int priority, int stackSize)
{
  NDFileGe *pPlugin =  new NDFileGe(portName,max_ge_bytes, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
                       priority, stackSize);
  //  return(asynSuccess);
  return pPlugin->start();

}
/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg initArg01 = { "max_ge_bytes",iocshArgInt};
static const iocshArg initArg1 = { "frame queue size",iocshArgInt};
static const iocshArg initArg2 = { "blocking callbacks",iocshArgInt};
static const iocshArg initArg3 = { "NDArrayPort",iocshArgString};
static const iocshArg initArg4 = { "NDArrayAddr",iocshArgInt};
static const iocshArg initArg5 = { "priority",iocshArgInt};
static const iocshArg initArg6 = { "stackSize",iocshArgInt};
static const iocshArg * const initArgs[] = {&initArg0,
                                            &initArg01,
                                            &initArg1,                                           
                                            &initArg2,
                                            &initArg3,
                                            &initArg4,
                                            &initArg5,
                                            &initArg6};
static const iocshFuncDef initFuncDef = {"NDFileGeConfigure",8,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
  drvNDFileGeConfigure(args[0].sval,args[1].ival, args[2].ival, args[3].ival,
                       args[4].sval,args[5].ival, 
                       args[6].ival, args[7].ival);
}

extern "C" void NDFileGeRegister(void)
{
  iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
  epicsExportRegistrar(NDFileGeRegister);
}

/* The constructor for this class */
//max_ge_bytes is size of NDArray thrown, 1 x GeLength bytes
#if ADCORE_VERSION>2
NDFileGe::NDFileGe(const char *portName,int max_ge_bytes ,int queueSize, int blockingCallbacks,
                       const char *NDArrayPort, int NDArrayAddr,
                       int priority, int stackSize) :
     NDPluginFile(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, 
                   500, 0, asynGenericPointerMask, asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize,10)

#else
NDFileGe::NDFileGe(const char *portName,int max_ge_bytes ,int queueSize, int blockingCallbacks,
                       const char *NDArrayPort, int NDArrayAddr,
                       int priority, int stackSize) :
     NDPluginFile(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, num_params,
                   500, 0, asynGenericPointerMask, asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize)
#endif
{
	int i;

    this->max_ge_bytes = max_ge_bytes;
	is_open_good=true;


	createParam(" ",asynParamInt32, & );

	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );
	createParam(" ",asynParamInt32, & );

	createParam(" ",asynParamFloat64, & );


	createParam(" ",asynParamFloat64, & );




    this->supportsMultipleArrays = 1;
    this->pAttributeId = NULL;


	setIntegerParam( , 0);

	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);
	setIntegerParam( , 0);






	my_array = 0;


    /* Set the plugin type string */
    setStringParam(NDPluginDriverPluginType, "NDFileGe");

    /* Try to connect to the array port */
    connectToArrayPort();

}

