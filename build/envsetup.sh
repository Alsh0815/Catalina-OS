REPO_DIR=$(pwd)
CMD_DIR="$REPO_DIR/build/cmd"
export CATALINA_PROJECT_DIR="$REPO_DIR"
export PATH="$CMD_DIR":$PATH

function init() {
    sudo apt update
    sudo apt upgrade
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

    sudo chmod 0777 ./build/cmd/emulate
    sudo chmod 0777 ./build/cmd/m

    cd edk2
    make -C ./BaseTools/Source/C
    cd ..
}

if [[ $1 = "--init" ]]; then
    init
fi

cd edk2
source edksetup.sh
cd ..

rm edk2/Conf/target.txt
cp -f ./build/edk2_target.txt ./edk2/Conf/target.txt

ln -fs ./$REPO_DIR/boot ./$REPO_DIR/edk2/CatalinaLoaderPkg
