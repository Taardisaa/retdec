Use this exact pattern now that runtime config/support is in place:

ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
./build-asan-clang14/src/retdec-decompiler/retdec-decompiler \
"/home/ruotoy/Workspace/retdec/examples/testbin/hello_world" \
-o "/home/ruotoy/Workspace/retdec/results/hello_world/hello_world.c" \
--no-memory-limit

If you want ASAN output saved to a file too:

mkdir -p "/home/ruotoy/Workspace/retdec/results/hello_world"
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:log_path=/home/ruotoy/Workspace/retdec/results/hello_world/asan \
./build-asan-clang14/src/retdec-decompiler/retdec-decompiler \
"/home/ruotoy/Workspace/retdec/examples/testbin/hello_world" \
-o "/home/ruotoy/Workspace/retdec/results/hello_world/hello_world.c" \
--no-memory-limit

Notes:
- keep --no-memory-limit (ASAN otherwise may hit mmap issues).
- if leak is found with halt_on_error=1, process exits non-zero