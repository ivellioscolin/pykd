#
# This is setup script for pykd project.
# Author: Maksim K. aka airmax
#

!define PRODUCT_ARCH "x86"

# When changing Python vesion don't forget to update ProductCode appropriately
!define PYTHON_INSTALLER "python-2.6.6.msi"
!define PYTHON_PRODUCT_CODE "{6151CF20-0BD8-4023-A4A0-6A86DCFE58E5}"

# Source http://www.microsoft.com/downloads/en/details.aspx?FamilyID=766a6af7-ec73-40ff-b072-9112bab119c2
# When changing VC runtime vesion don't forget to update ProductCode appropriately
!define VCRUNTIME_URL "http://download.microsoft.com/download/6/B/B/6BB661D6-A8AE-4819-B79F-236472F6070C/vcredist_x86.exe"
!define VCRUNTIME_PRODUCT_CODE "{837b34e3-7c30-493c-8f6a-2b0f04e2912c}"

!include "pykd_setup_common.nsh"
