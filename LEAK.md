# Memory Leak Analysis Log (ASAN)

## Environment and Build

- Repository: `retdec`
- Compiler: `clang++-14`
- Build type: `Release`
- Sanitizer flags:
  - `-fsanitize=address`
  - `-fno-omit-frame-pointer`
  - `-g`
- Runtime ASAN options used:
  - `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1`

## Important Runtime Behavior Found First

Before leak triage, decompilation produced no `.c` output and no explicit error. Root cause:

- `retdec-decompiler` expects runtime config at path relative to executable:
  - `../share/retdec/decompiler-config.json`
  - source reference: `src/retdec-decompiler/retdec-decompiler.cpp` (config loading around lines 996-1005)
- If the file is missing, code silently continues (no warning) and may exit successfully after early phases without output.

To get meaningful leak runs in build-tree execution, runtime layout was prepared under:

- `build-asan-clang14/src/share/retdec/decompiler-config.json`
- `build-asan-clang14/src/share/retdec/support` -> symlink to repo `support/`

## Reproduction Command Pattern

```bash
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
./build-asan-clang14/src/retdec-decompiler/retdec-decompiler \
"/home/ruotoy/Workspace/retdec/examples/testbin/hello_world" \
-o "/home/ruotoy/Workspace/retdec/results/hello_world/hello_world.c" \
--no-memory-limit
```

Notes:

- `--no-memory-limit` was required to avoid ASAN mmap failures (`ERROR: Failed to mmap`) in this setup.

## Confirmed Leak Symptoms

LeakSanitizer reports direct leaks during decompilation after full pass pipeline runs.

Representative top frames repeatedly include:

- `retdec::bin2llvmir::SymbolicTree::SymbolicTree(...)`
  - `src/bin2llvmir/analyses/symbolic_tree.cpp:75`
- `retdec::bin2llvmir::SymbolicTree::expandNode(...)`
  - `src/bin2llvmir/analyses/symbolic_tree.cpp:329`

Observed caller paths in leak stacks:

1. Constants optimization path
   - `retdec::bin2llvmir::ConstantsAnalysis::checkForGlobalInInstruction(...)`
   - `src/bin2llvmir/optimizations/constants/constants.cpp:126`

2. Stack optimization path
   - `retdec::bin2llvmir::StackAnalysis::handleInstruction(...)`
   - `src/bin2llvmir/optimizations/stack/stack.cpp:140`

3. Conditional branch optimization path
   - `retdec::bin2llvmir::CondBranchOpt::runOnInstruction(...)`
   - `src/bin2llvmir/optimizations/cond_branch_opt/cond_branch_opt.cpp:100`

## Isolation Check Performed

To test whether leaks are only from `retdec-constants`, a temporary config variant removed that pass.

Result:

- Leaks still reproduced.
- New dominant stacks still pointed to `SymbolicTree` allocation/expansion, now through `stack` and `cond_branch_opt` paths.

Conclusion from this isolation:

- Leak source is not unique to the constants pass.
- Leak appears tied to shared SymbolicTree lifecycle/ownership patterns used by multiple bin2llvmir optimizations.

## Current Root-Cause Conclusion

- Primary issue: memory not reclaimed for some SymbolicTree allocations/graphs reachable from multiple optimization workflows.
- Most actionable hotspot for code-level investigation starts in:
  - `src/bin2llvmir/analyses/symbolic_tree.cpp` (constructor + recursive expansion)
- Secondary issue (separate from leak): silent missing-runtime-config behavior causes misleading success-without-output execution.

## Exact Code Snippets Triggering Leak Allocations

These call sites create SymbolicTree graphs that allocate and trigger the observed ASAN leaks.

### Pass Call Sites (SymbolicTree creation)

- Constants optimization:
  - `src/bin2llvmir/optimizations/constants/constants.cpp:126`
  - `auto root = SymbolicTree::PrecomputedRda(RDA, val);`

- Stack optimization:
  - `src/bin2llvmir/optimizations/stack/stack.cpp:140`
  - `auto root = SymbolicTree::PrecomputedRdaWithValueMap(RDA, val, &val2val);`

- Conditional branch optimization:
  - `src/bin2llvmir/optimizations/cond_branch_opt/cond_branch_opt.cpp:100`
  - `auto root = SymbolicTree::PrecomputedRda(RDA, cond);`

- Decoder paths (also instantiate SymbolicTree):
  - `src/bin2llvmir/optimizations/decoder/decoder.cpp:828`
  - `auto st = SymbolicTree::OnDemandRda(val, 20);`

### Allocation Sites (where heap allocations occur)

- `src/bin2llvmir/analyses/symbolic_tree.cpp:75`
  - `ops.reserve(_naryLimit);`
- `src/bin2llvmir/analyses/symbolic_tree.cpp:224`
  - `ops.emplace_back(...);`
- `src/bin2llvmir/analyses/symbolic_tree.cpp:253`
  - `ops.emplace_back(...);`
- `src/bin2llvmir/analyses/symbolic_tree.cpp:329`
  - `ops.emplace_back(...);`

These allocation points match the top frames in LeakSanitizer traces.

## Artifacts and Outputs

- Example generated output after runtime fix:
  - `results/hello_world/hello_world.c`
  - `results/hello_world/hello_world.ll`
  - `results/hello_world/hello_world.bc`
  - `results/hello_world/hello_world.dsm`
  - `results/hello_world/hello_world.config.json`

- Temporary analysis helper files created during investigation:
  - `results/no_constants_config.json`
  - `results/decompiler-config.backup.json`

## Suggested Next Debug Step

- Add focused instrumentation around SymbolicTree construction/destruction/ownership boundaries to identify which trees are retained at process end and by which pass flow.
