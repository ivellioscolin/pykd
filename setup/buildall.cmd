py -2.7 setup.py bdist_zip --plat-name=win32
py -2.7 setup.py bdist_zip --plat-name=win-amd64
py -2.7 setup.py bdist_wheel --plat-name=win32
py -2.7 setup.py bdist_wheel --plat-name=win-amd64

py -3.5 setup.py bdist_zip --plat-name=win32
py -3.5 setup.py bdist_zip --plat-name=win-amd64
py -3.5 setup.py bdist_wheel --plat-name=win32
py -3.5 setup.py bdist_wheel --plat-name=win-amd64