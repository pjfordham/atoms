# atoms
Port of an old Amiga game from the 90's

# build and run instructions
mkdir debug
cd debug
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug ..
cd ..
./debug/Atoms
