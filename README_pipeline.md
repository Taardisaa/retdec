# RetDec Decompilation Pipeline

This document describes the complete decompilation pipeline used in RetDec, based on the implementation in `src/retdec/retdec.cpp`.

## Pipeline Overview

The RetDec decompilation process follows a three-stage pipeline:

```
Binary → LLVM IR → High-Level Language (C-like pseudocode)
```

All transformations are performed using LLVM's pass infrastructure, combining both standard LLVM optimization passes and RetDec's custom decompilation-specific passes.

## Stage 1: Binary to LLVM IR (bin2llvmir)

### Input
- Raw binary executable (ELF, PE, Mach-O, etc.)

### Process
The binary-to-IR conversion is handled by RetDec's custom LLVM passes:

1. **ProviderInitialization Pass** (`bin2llvmir::ProviderInitialization`)
   - Initializes the decompilation configuration
   - Sets up providers for managing decompilation metadata
   - Configures architecture-specific settings

2. **Decoder Pass** (`bin2llvmir::Decoder`)
   - Decodes machine code instructions into LLVM IR
   - Uses Capstone disassembler internally
   - Reconstructs control flow graphs (CFGs)
   - Handles architecture-specific instruction semantics
   - Creates basic blocks and function boundaries

### Output
- LLVM IR module with:
  - Functions corresponding to detected binary functions
  - Basic blocks with decoded instructions
  - Metadata preserving binary addresses and instruction information

## Stage 2: LLVM IR Optimization

### Input
- Raw LLVM IR from Stage 1

### Process
The decompilation pipeline runs a configurable sequence of passes specified in `config.parameters.llvmPasses`. This includes:

#### RetDec Custom Passes
- Binary analysis and reconstruction passes
- Architecture-specific idiom recognition
- Stack and register variable recovery
- Type reconstruction
- Control flow structuring

#### Standard LLVM Passes
The pipeline leverages LLVM's extensive optimization infrastructure:
- **Scalar optimizations**: Constant propagation, dead code elimination, instruction combining
- **Interprocedural optimizations (IPO)**: Inlining, global optimization
- **Analysis passes**: Call graph analysis, scalar evolution, target library info
- **Transform utilities**: CFG simplification, loop transformations

#### Library Function Protection
To prevent unwanted optimizations that would harm decompilation quality:
```cpp
TLII.disableAllFunctions();  // Prevents printf() → puts() and similar transformations
```

### Configuration
Pass execution is controlled by:
- `config.parameters.llvmPasses` - ordered list of pass names
- Pass-specific configuration via `setConfig()`
- Phase tracking via `ModulePassPrinter` for logging

### Output
- Optimized and analyzed LLVM IR ready for high-level code generation

## Stage 3: LLVM IR to High-Level Language (llvmir2hll)

### Input
- Optimized LLVM IR from Stage 2

### Process
The final conversion is performed by the **LlvmIr2Hll Pass** (`llvmir2hll::LlvmIr2Hll`):

1. **IR Analysis**
   - Analyzes LLVM IR structure
   - Identifies high-level constructs (loops, conditionals, etc.)
   
2. **Code Generation**
   - Converts LLVM IR to Abstract Syntax Tree (AST)
   - Reconstructs high-level control structures (if/else, while, for, switch)
   - Generates variable declarations and type annotations
   - Produces C-like pseudocode

3. **Output**
   - The generated code is written to the `outString` parameter passed to `decompile()`

### Output
- Human-readable C-like pseudocode representing the decompiled binary

## Implementation Details

### Pass Management
All transformations use LLVM's `legacy::PassManager`:
```cpp
llvm::legacy::PassManager pm;
pm.add(new TargetLibraryInfoWrapperPass(TLII));
// Add configured passes...
pm.run(*module);  // Execute all passes sequentially
```

### Pass Registry
LLVM passes are initialized via `initializeLlvmPasses()`:
- Core IR passes
- Scalar optimizations
- Interprocedural optimizations (IPO)
- Analysis infrastructure
- Transform utilities
- Instruction combining
- Target-specific passes

### Phase Tracking
The `ModulePassPrinter` class provides logging for pipeline execution:
- Tracks which phase is currently executing
- Distinguishes between RetDec passes (prefixed with "retdec") and LLVM passes
- Aggregates LLVM passes under a single "LLVM" phase for cleaner logging

### Configuration Flow
1. Configuration is passed to `decompile()` via `retdec::config::Config`
2. Binary input path and pass list are set in config parameters
3. `ProviderInitialization` receives config and sets up the decompilation context
4. `LlvmIr2Hll` receives config for output formatting and language settings

## Key Architectural Decisions

### Why LLVM IR?
Using LLVM IR as an intermediate representation provides:
- **Rich optimization infrastructure**: Leverage decades of compiler research
- **Architecture independence**: Uniform representation across x86, ARM, MIPS, etc.
- **Mature tooling**: Analysis, transformation, and debugging capabilities
- **SSA form**: Simplified data flow analysis

### Hybrid Pass Approach
Combining LLVM's standard passes with RetDec's custom passes:
- **Standard LLVM passes**: Handle general optimizations efficiently
- **Custom RetDec passes**: Address decompilation-specific challenges
  - Binary metadata preservation
  - Architecture-specific idioms
  - High-level structure reconstruction

### Configurable Pipeline
Pass sequence is externally configurable rather than hardcoded:
- Flexibility for different binary types and analysis goals
- Easy experimentation with pass ordering
- Ability to skip unnecessary passes for performance

## Related Functions

### `disassemble()`
A simplified version of the pipeline that only performs Stage 1 (Binary → LLVM IR):
- Used when only disassembly is needed, not full decompilation
- Returns the LLVM IR module and function metadata
- Skips optimization and high-level code generation

### `createLlvmModule()`
Creates an empty LLVM IR module as the starting point:
- Initializes minimal valid LLVM module
- Runs verifier to ensure structural correctness
- Provides clean slate for binary decoding

## Example Flow

For a simple binary function:

1. **Binary**: `push ebp; mov ebp, esp; mov eax, [ebp+8]; ret`

2. **LLVM IR** (after Stage 1):
   ```llvm
   define i32 @func_0x401000(i32 %arg1) {
     %stack = alloca i32
     store i32 %arg1, i32* %stack
     %result = load i32, i32* %stack
     ret i32 %result
   }
   ```

3. **Optimized IR** (after Stage 2):
   ```llvm
   define i32 @func_0x401000(i32 %arg1) {
     ret i32 %arg1
   }
   ```

4. **C Code** (after Stage 3):
   ```c
   int func_0x401000(int arg1) {
       return arg1;
   }
   ```

## Performance Considerations

- **Memory**: Empty module starts small, grows as binary is decoded
- **Pass ordering**: Critical for both quality and performance
- **Library optimization control**: Prevents unwanted transformations that complicate output
- **Incremental processing**: Each pass transforms the IR in place

## Future Enhancements

Based on code comments:
- Migration from `legacy::PassManager` to new pass manager
- Better integration between `decompile()` and `retdec-decompiler.cpp` (currently duplicated)
- More sophisticated type reconstruction
- Improved handling of architecture-specific idioms
