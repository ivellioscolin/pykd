#
# This is setup script for pykd project.
# Author: Maksim K. aka airmax
#

!define PRODUCT_ARCH "x86"

# When changing Python vesion don't forget to update ProductCode appropriately
!define PYTHON_INSTALLER "python-2.6.6.msi"
!define PYTHON_PRODUCT_CODE "{6151CF20-0BD8-4023-A4A0-6A86DCFE58E5}"

# When changing VC runtime vesion don't forget to update ProductCode appropriately
!define VCRUNTIME_PRODUCT_CODE "{710f4c1c-cc18-4c49-8cbf-51240c89a1a2}"

!include "pykd_setup_common.nsh"
