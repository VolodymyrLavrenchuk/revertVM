/* *********************************************************************
 * Copyright (c) 2007 VMware, Inc.
 * All rights not expressly granted to you by VMware, Inc. are reserved.
 * *********************************************************************/

/* This demonstrates how to open a virtual machine, create
 * a snapshot, and revert the snapshot. The virtual machine
 * will remain powered on at the end of this program.
 *
 * This uses the VixJob_Wait function to block after starting each
 * asynchronous function. This effectively makes the asynchronous
 * functions synchronous, because VixJob_Wait will not return until the
 * asynchronous function has completed.
 */

#include <stdio.h>
#include <stdlib.h>

#include "vix.h"


int
main(int argc, char **argv)
{
	char* hostname = argv[1];
	char* username = argv[2];
	char* password = argv[3];
	char* vmxPath = argv[4];
	char* snName = argv[5];
	
	VixError err = 0;
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;
    VixHandle snapshotHandle = VIX_INVALID_HANDLE;

    jobHandle = VixHost_Connect(VIX_API_VERSION,
                                VIX_SERVICEPROVIDER_VMWARE_VI_SERVER,
                                hostname, // *hostName,
                                0, // hostPort,
                                username, // *userName,
                                password, // *password,
                                0, // options,
                                VIX_INVALID_HANDLE, // propertyListHandle,
                                NULL, // *callbackProc,
                                NULL); // *clientData);
    err = VixJob_Wait(jobHandle, 
                      VIX_PROPERTY_JOB_RESULT_HANDLE, 
                      &hostHandle,
                      VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        goto abort;
    }

    Vix_ReleaseHandle(jobHandle);
    jobHandle = VixVM_Open(hostHandle,
                           vmxPath,
                           NULL, // VixEventProc *callbackProc,
                           NULL); // void *clientData);
    err = VixJob_Wait(jobHandle, 
                      VIX_PROPERTY_JOB_RESULT_HANDLE, 
                      &vmHandle,
                      VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        goto abort;
    }
    Vix_ReleaseHandle(jobHandle);


    err = VixVM_GetNamedSnapshot(vmHandle, snName, &snapshotHandle);
    if (VIX_FAILED(err)) {
        goto abort;
    }
    /*
     * Here we pass VMPOWEROPTIONS to RevertToSnapshot since we
     * took a snapshot of a powered on virtual machine, and the
     * virtual machine will then be powered on again when we revert.
     */
    jobHandle = VixVM_RevertToSnapshot(vmHandle, 
                                       snapshotHandle,
                                       VIX_VMPOWEROP_NORMAL, // options,
                                       VIX_INVALID_HANDLE,
                                       NULL, // *callbackProc,
                                       NULL); // *clientData);
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        goto abort;
    }


abort:
    Vix_ReleaseHandle(jobHandle);
    Vix_ReleaseHandle(vmHandle);
    Vix_ReleaseHandle(snapshotHandle);

    VixHost_Disconnect(hostHandle);

    return err;
}


