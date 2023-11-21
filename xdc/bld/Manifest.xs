/*
 *  ======== getAllFiles ========
 */
function getAllFiles(excludeDirs)
{
    if (xdc.om.$name != "rel") {
        throw new Error(
            "Manifest.getAllFiles can only be called from within a release script");
    }

    /* clear current files list */
    this.files = [];

    /* re-populate based on excludeDirs */
    /* rcl is alredy loaded by the release model initialization rcl.js */
    rcl.getFiles(this.files, excludeDirs);
}
/*
 *  @(#) xdc.bld; 1, 0, 2,0; 12-9-2015 17:35:10; /db/ztree/library/trees/xdc/xdc-B06/src/packages/
 */

