#
# This is setup script for pykd project.
# Author: Maksim K. aka airmax
#

!define PRODUCT_ARCH "x64"

# When changing Python vesion don't forget to update ProductCode appropriately
!define PYTHON_INSTALLER "python-2.6.6.amd64.msi"
!define PYTHON_PRODUCT_CODE "{6151CF20-0BD8-4023-A4A0-6A86DCFE58E6}"

# When changing VC runtime vesion don't forget to update ProductCode appropriately
!define VCRUNTIME_PRODUCT_CODE "{ad8a2fa1-06e7-4b0d-927d-6e54b3d31028}"

!include "pykd_setup_common.nsh"
