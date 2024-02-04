cd build
cmake --build . --config Release
cd ..

printf "\n\nStarting voxel-engine\n\n"

./build/Release/voxel-engine.exe