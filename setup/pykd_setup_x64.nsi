#
# This is setup script for pykd project.
# Author: Maksim K. aka airmax
#

!define PRODUCT_ARCH "x64"

# When changing Python vesion don't forget to update ProductCode appropriately
!define PYTHON_INSTALLER "python-2.6.6.amd64.msi"
!define PYTHON_PRODUCT_CODE "{6151CF20-0BD8-4023-A4A0-6A86DCFE58E6}"

# Source http://www.microsoft.com/downloads/en/details.aspx?FamilyID=766a6af7-ec73-40ff-b072-9112bab119c2
# When changing VC runtime vesion don't forget to update ProductCode appropriately
!define VCRUNTIME_URL "http://download.microsoft.com/download/6/B/B/6BB661D6-A8AE-4819-B79F-236472F6070C/vcredist_x64.exe"
!define VCRUNTIME_PRODUCT_CODE "{6ce5bae9-d3ca-4b99-891a-1dc6c118a5fc}"

!include "pykd_setup_common.nsh"
