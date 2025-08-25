FROM ubuntu:jammy
WORKDIR /app

# Install packages for LLVM and SDL3.

RUN apt update && apt -y install wget lsb-release software-properties-common gnupg build-essential git make pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev

# Install LLVM.

RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

# Install a recent version of CMake.

RUN wget https://github.com/Kitware/CMake/releases/download/v4.1.0/cmake-4.1.0-linux-x86_64.tar.gz
RUN mkdir cmake
RUN tar -xf cmake-4.1.0-linux-x86_64.tar.gz -C cmake --strip-components=1
RUN rm cmake-4.1.0-linux-x86_64.tar.gz