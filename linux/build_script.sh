#! /bin/sh

git clone https://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@github.com/TRDario/Bodge.git
cd Bodge
../cmake/bin/cmake -G "Ninja Multi-Config" -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang-20 -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++-20 -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -S . -B build
../cmake/bin/cmake --build build --config Release --target all --
cp -R build/final/* /output