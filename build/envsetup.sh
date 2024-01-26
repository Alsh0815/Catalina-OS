REPO_DIR=$(pwd)
CMD_DIR="$REPO_DIR/build/cmd"
export PATH="$CMD_DIR":$PATH

CLANG_VERSION=10
LLD_VERSION=7
LLVM_VERSION=14

function init() {
    apt_list=(
        build-essential
        clang-$CLANG_VERSION
        lld-$LLD_VERSION
        llvm-$LLVM_VERSION-dev
        nasm
        python3-distutils
        qemu-system-x86
        qemu-utils
        uuid-dev
    )
    for var in ${apt_list[@]}; do
        is_installed=$(which $var)
        sudo apt install $var
    done

    clang_list=(
        clang
        clang++
        clang-cpp
    )

    for var in ${clang_list[@]}; do
        ln -s /usr/bin/$var-$CLANG_VERSION /usr/bin/$var >/dev/null 2>&1
    done

    lld_list=(
        ld.lld
        lld-link
    )

    for var in ${lld_list[@]}; do
        ln -s /usr/bin/$var-$LLD_VERSION /usr/bin/$var >/dev/null 2>&1
    done

    llvm_list=(
        llvm-PerfectShuffle
        llvm-ar
        llvm-as
        llvm-bcanalyzer
        llvm-cat
        llvm-cfi-verify
        llvm-config
        llvm-cov
        llvm-c-test
        llvm-cvtres
        llvm-cxxdump
        llvm-cxxfilt
        llvm-diff
        llvm-dis
        llvm-dlltool
        llvm-dwarfdump
        llvm-dwp
        llvm-exegesis
        llvm-extract
        llvm-lib
        llvm-link
        llvm-lto
        llvm-lto2
        llvm-mc
        llvm-mca
        llvm-modextract
        llvm-mt
        llvm-nm
        llvm-objcopy
        llvm-objdump
        llvm-opt-report
        llvm-pdbutil
        llvm-ranlib
        llvm-rc
        llvm-readelf
        llvm-readobj
        llvm-rtdyld
        llvm-size
        llvm-split
        llvm-stress
        llvm-strings
        llvm-strip
        llvm-symbolizer
        llvm-tblgen
        llvm-undname
        llvm-xray
    )

    for var in ${llvm_list[@]}; do
        ln -s /usr/bin/$var-$LLVM_VERSION /usr/bin/$var >/dev/null 2>&1
    done

    ln -s boot edk2/CatalinaLoaderPkg >/dev/null 2>&1
}

if [[ $1 = "--init" ]]; then
    init
fi

cd edk2
source edksetup.sh
cd ..

rm edk2/Conf/target.txt
cp -f build/edk2_target.txt edk2/Conf/target.txt
