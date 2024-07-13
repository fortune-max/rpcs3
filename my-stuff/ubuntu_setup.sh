# get 6.6.3 to CWD
if [ ! -d "./6.6.3" ]; then
    mkdir -p "/tmp/Qt"
    mkdir -p "./6.6.3/linux"
    cd 6.6.3 && ln -s linux clang_64 && cd ..
    git -C /tmp/Qt clone https://github.com/engnr/qt-downloader.git
    git -C /tmp/Qt/qt-downloader checkout f52efee0f18668c6d6de2dec0234b8c4bc54c597
    export PIPENV_PIPFILE="/tmp/Qt/qt-downloader/Pipfile"
    pipenv install
    pipenv run /tmp/Qt/qt-downloader/qt-downloader linux desktop 6.6.3 gcc_64 --opensource --addons qtmultimedia
fi
# export Qt6_DIR="$PWD/6.6.3/linux/lib/cmake/Qt6" # not needed
export PATH="$PWD/6.6.3/linux/bin:$PATH"

# readme dependencies (not sure why vulkan is here as well as below :/, maybe vulkan-dev is different from vulkan-sdk)
sudo apt-get install build-essential libasound2-dev libpulse-dev libopenal-dev libglew-dev zlib1g-dev libedit-dev libvulkan-dev libudev-dev git libevdev-dev libsdl2-2.0 libsdl2-dev libjack-dev libsndio-dev
# qt dependency
sudo apt-get install -y libxcb-cursor-dev

# cmake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ $UBUNTU_CODENAME main"
sudo apt-get update
sudo apt-get install cmake

# vulkan
wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add -
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.268-$UBUNTU_CODENAME.list https://packages.lunarg.com/vulkan/1.3.268/lunarg-vulkan-1.3.268-$UBUNTU_CODENAME.list
sudo apt-get update
sudo apt install vulkan-sdk

# llvm
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy main" >> /etc/apt/sources.list
sudo apt-get update
sudo apt install llvm-19-dev

cmake ../rpcs3
make 

# for compiling anything with ffmpeg (eg frame consumer outside project)
sudo apt-get install libavcodec-dev libavformat-dev # libavutil-dev libswscale-dev libavresample-dev libavdevice-dev

sudo apt-get install libssl-dev # verify checksum of AVFrame

g++ reader.cpp -I/usr/include/x86_64-linux-gnu -lavutil -o reader && ./reader

# or

g++ reader.cpp `pkg-config --cflags --libs libavutil` -o reader && ./reader