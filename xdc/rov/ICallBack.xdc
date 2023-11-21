/* 
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * */
package xdc.rov

/*!
 *  ======== ICallBack ========
 *  ROV tooling progress callback interface
 */
metaonly interface ICallBack {

instance:

    /*!
     *  ======== updateStartupProgress ========
     *  Progress update callback
     *  
     *  The ROV model calls this API at particular points to update any
     *  startup progress provided by the client ROV tooling.
     */
    Void updateStartupProgress(Int percent, String message);
    
    /*!
     *  ======== getAbortFlag ========
     */
    Bool getAbortFlag();
}

/*
 *  @(#) xdc.rov; 1, 0, 1,0; 12-9-2015 17:36:22; /db/ztree/library/trees/xdc/xdc-B06/src/packages/
 */

