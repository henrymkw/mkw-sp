#include <Common.S>

#ifdef DEBUG

PATCH_REPLACE_START(LicenseManagementPage_onDeleteLicenseButtonClick, 0x0)
    // Return when in debug to prevent accidental deletion of license
    blr
PATCH_REPLACE_END(LicenseManagementPage_onDeleteLicenseButtonClick, 0x0)

#endif
