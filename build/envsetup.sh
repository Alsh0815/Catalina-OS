REPO_DIR=$(pwd)
CMD_DIR="$REPO_DIR/build/cmd"
export PATH="$CMD_DIR":$PATH

function init() {
    apt_list=(
        build-essential
        clang
        lld
        llvm
        llvm-dev
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
}

if [[ $1 = "--init" ]]; then
    init
fi

cd edk2
source edksetup.sh
make -C ./BaseTools/Source/C
cd ..

rm edk2/Conf/target.txt
cp -f build/edk2_target.txt edk2/Conf/target.txt

ln -fs $REPO_DIR/boot $REPO_DIR/edk2/CatalinaLoaderPkg
