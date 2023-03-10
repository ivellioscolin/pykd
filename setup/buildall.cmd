py -2.7 setup.py bdist_zip --plat-name=win32
py -2.7 setup.py bdist_zip --plat-name=win-amd64
py -2.7 setup.py bdist_wheel --plat-name=win32 --python-tag=cp27
py -2.7 setup.py bdist_wheel --plat-name=win-amd64 --python-tag=cp27

py -3.5 setup.py bdist_zip --plat-name=win32
py -3.5 setup.py bdist_zip --plat-name=win-amd64
py -3.5 setup.py bdist_wheel --plat-name=win32 --python-tag=cp35
py -3.5 setup.py bdist_wheel --plat-name=win-amd64 --python-tag=cp35

py -3.6 setup.py bdist_zip --plat-name=win32
py -3.6 setup.py bdist_zip --plat-name=win-amd64
py -3.6 setup.py bdist_wheel --plat-name=win32 --python-tag=cp36
py -3.6 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp36

py -3.7 setup.py bdist_zip --plat-name=win32
py -3.7 setup.py bdist_zip --plat-name=win-amd64
py -3.7 setup.py bdist_wheel --plat-name=win32 --python-tag=cp37
py -3.7 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp37

py -3.8 setup.py bdist_zip --plat-name=win32
py -3.8 setup.py bdist_zip --plat-name=win-amd64
py -3.8 setup.py bdist_wheel --plat-name=win32 --python-tag=cp38
py -3.8 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp38

py -3.9 setup.py bdist_zip --plat-name=win32
py -3.9 setup.py bdist_zip --plat-name=win-amd64
py -3.9 setup.py bdist_wheel --plat-name=win32 --python-tag=cp39
py -3.9 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp39

py setup.py bdist_pdb