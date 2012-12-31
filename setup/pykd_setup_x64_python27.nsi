#
# This is setup script for pykd project.
# Author: Maksim K. aka airmax
#

!define PRODUCT_ARCH "x64"

# When changing Python vesion don't forget to update ProductCode appropriately
!define PYTHON_VERSION "2.7"
!define PYTHON_URL_BASE "http://python.org/ftp/python/2.7.3/"
!define PYTHON_INSTALLER "python-2.7.3.amd64.msi"
!define PYTHON_PRODUCT_CODE "{C0C31BCC-56FB-42A7-8766-D29E1BD74C7D}"

# When changing VC runtime vesion don't forget to update ProductCode appropriately
# VC9 (VS2008) SP1 + MFC Security Update
!define VCRUNTIME_PRODUCT_CODE "{5FCE6D76-F5DC-37AB-B2B8-22AB8CEDB1D4}"

!define MSDIA_DLL_NAME "msdia90.dll"
!define CLSID_DiaSource "{4C41678E-887B-4365-A09E-925D28DB33C2}"

!include "pykd_setup_common.nsh"
