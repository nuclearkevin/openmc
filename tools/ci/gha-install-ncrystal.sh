#!/bin/bash
set -ex
cd $HOME

#Use the NCrystal develop branch (in the near future we can move this to master):
git clone https://github.com/mctools/ncrystal --branch develop --single-branch --depth 1 ncrystal_src

SRC_DIR="$PWD/ncrystal_src"
BLD_DIR="$PWD/ncrystal_bld"
INST_DIR="$PWD/ncrystal_inst"
PYTHON=$(which python3)

CPU_COUNT=1

mkdir "$BLD_DIR"
cd ncrystal_bld

#cmake -Dstatic=on .. && make 2>/dev/null && sudo make install

cmake \
    "${SRC_DIR}" \
    -DBUILD_SHARED_LIBS=ON \
    -DNCRYSTAL_NOTOUCH_CMAKE_BUILD_TYPE=ON \
    -DNCRYSTAL_MODIFY_RPATH=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DNCRYSTAL_ENABLE_EXAMPLES=OFF \
    -DNCRYSTAL_ENABLE_SETUPSH=OFF \
    -DNCRYSTAL_ENABLE_DATA=OFF \
    -DNCRYSTAL_SKIP_PYMODINST=ON \
    -DCMAKE_INSTALL_PREFIX="${INST_DIR}" \
    -DPython3_EXECUTABLE="$PYTHON"

make -j${CPU_COUNT:-1}
make install

#Note: There is no "make test" or "make ctest" functionality for NCrystal
#      yet. If it appears in the future, we should add it here.


#The next stuff is not pretty, but it is needed as a temporary
#workaround until NCrystal add better support for system-wide
#installations in CMake:

cat <<EOF > ./findncrystallib.py
import pathlib
libname = pathlib.Path('./cfg_ncrystal_libname.txt').read_text().strip()
libs = list(pathlib.Path("${INST_DIR}").glob("**/lib*/**/%s"%libname))
assert len(libs)==1
pathlib.Path('ncrystal_liblocation.txt').write_text(str(libs[0]))
EOF

python3 ./findncrystallib.py

TMPNCRYSTALLIBLOCATION=$(cat ./ncrystal_liblocation.txt)

cat <<EOF > ./ncrystal_pypkg/NCrystal/_nclibpath.py
#File autogenerated for working with systemwide install of NCrystal:
import pathlib
liblocation = pathlib.Path('${TMPNCRYSTALLIBLOCATION}'.strip())
EOF

$PYTHON -m pip install ./ncrystal_pypkg/ -vv

eval $( "${INST_DIR}/bin/ncrystal-config --setup" )

