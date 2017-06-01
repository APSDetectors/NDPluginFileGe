/*
 * drvNDFileGe.h
 *
 * Asyn driver for callbacks to write data to Ge files for area detectors.
 *
 * Author: Tim Madden (thanks to J. Hammonds, B. Tieman)
 *
 * Created Nov 4, 2009
 */

#ifndef DRV_NDFileGe_H
#define DRV_NDFileGe_H

#ifdef __cplusplus
extern "C" {
#endif

int drvNDFileGeConfigure(const char *portName,int max_imm_bytes, int queueSize, int blockingCallbacks,
                             const char *NDArrayPort, int NDArrayAddr,
                             int priority, int stackSize);

#ifdef __cplusplus
}
#endif
#endif
