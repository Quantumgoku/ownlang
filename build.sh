cmake --build build/
echo -e "Build complete. Running the program...\n"
./build/hydro "$@"