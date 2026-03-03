#!/usr/bin/env python3

"""Differential CSmith test runner for two RetDec builds."""

import argparse
import json
import os
import random
import shlex
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional


@dataclass
class CmdResult:
    returncode: int
    stdout: str
    timeout: bool


def run_cmd(cmd: List[str], timeout_s: int, cwd: Optional[Path] = None) -> CmdResult:
    try:
        completed = subprocess.run(
            cmd,
            cwd=str(cwd) if cwd else None,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=timeout_s,
            check=False,
        )
        return CmdResult(completed.returncode, completed.stdout, False)
    except subprocess.TimeoutExpired as exc:
        output = exc.stdout or ""
        return CmdResult(124, output, True)


def ensure_executable(path: Path, name: str) -> None:
    if not path.exists():
        raise FileNotFoundError(f"{name} not found: {path}")
    if not os.access(path, os.X_OK):
        raise PermissionError(f"{name} is not executable: {path}")


def generate_source(
    csmith_bin: Path, seed: int, out_c: Path, timeout_s: int
) -> CmdResult:
    cmd = [str(csmith_bin), "--seed", str(seed)]
    res = run_cmd(cmd, timeout_s)
    if res.returncode == 0 and not res.timeout:
        out_c.write_text(res.stdout, encoding="utf-8")
    return res


def compile_c(
    source: Path, output_bin: Path, csmith_include: Path, timeout_s: int
) -> CmdResult:
    cmd = [
        "gcc",
        str(source),
        "-I",
        str(csmith_include),
        "-O0",
        "-w",
        "-o",
        str(output_bin),
    ]
    return run_cmd(cmd, timeout_s)


def decompile_binary(
    retdec_bin: Path, input_bin: Path, out_c: Path, timeout_s: int
) -> CmdResult:
    cmd = [
        str(retdec_bin),
        str(input_bin),
        "--cleanup",
        "--silent",
        "-o",
        str(out_c),
    ]
    return run_cmd(cmd, timeout_s)


def run_binary(binary: Path, timeout_s: int) -> CmdResult:
    return run_cmd([str(binary)], timeout_s)


def semantic_key(run_result: CmdResult) -> str:
    return f"rc={run_result.returncode};timeout={int(run_result.timeout)};out={run_result.stdout.strip()}"


def short(text: str, max_len: int = 220) -> str:
    text = text.replace("\n", " ").strip()
    if len(text) <= max_len:
        return text
    return text[:max_len] + "..."


def process_one(
    seed: int,
    case_dir: Path,
    csmith_bin: Path,
    csmith_include: Path,
    retdec_a: Path,
    retdec_b: Path,
    timeout_gen: int,
    timeout_compile: int,
    timeout_decompile: int,
    timeout_run: int,
) -> Dict[str, object]:
    case_dir.mkdir(parents=True, exist_ok=True)
    source_c = case_dir / "sample.c"
    orig_bin = case_dir / "sample.bin"
    a_c = case_dir / "a.c"
    b_c = case_dir / "b.c"
    a_bin = case_dir / "a.bin"
    b_bin = case_dir / "b.bin"

    result: Dict[str, object] = {
        "seed": seed,
        "status": "init",
        "consistent": False,
    }

    gen = generate_source(csmith_bin, seed, source_c, timeout_gen)
    result["generate"] = {
        "rc": gen.returncode,
        "timeout": gen.timeout,
        "log": short(gen.stdout),
    }
    if gen.returncode != 0 or gen.timeout:
        result["status"] = "csmith_failed"
        return result

    cc_orig = compile_c(source_c, orig_bin, csmith_include, timeout_compile)
    result["compile_original"] = {
        "rc": cc_orig.returncode,
        "timeout": cc_orig.timeout,
        "log": short(cc_orig.stdout),
    }
    if cc_orig.returncode != 0 or cc_orig.timeout:
        result["status"] = "compile_original_failed"
        return result

    run_orig = run_binary(orig_bin, timeout_run)
    result["run_original"] = {
        "rc": run_orig.returncode,
        "timeout": run_orig.timeout,
        "out": run_orig.stdout.strip(),
    }

    dec_a = decompile_binary(retdec_a, orig_bin, a_c, timeout_decompile)
    result["decompile_a"] = {
        "rc": dec_a.returncode,
        "timeout": dec_a.timeout,
        "log": short(dec_a.stdout),
    }

    dec_b = decompile_binary(retdec_b, orig_bin, b_c, timeout_decompile)
    result["decompile_b"] = {
        "rc": dec_b.returncode,
        "timeout": dec_b.timeout,
        "log": short(dec_b.stdout),
    }

    if dec_a.returncode != 0 or dec_a.timeout or not a_c.exists():
        result["status"] = "decompile_a_failed"
        return result
    if dec_b.returncode != 0 or dec_b.timeout or not b_c.exists():
        result["status"] = "decompile_b_failed"
        return result

    cc_a = compile_c(a_c, a_bin, csmith_include, timeout_compile)
    cc_b = compile_c(b_c, b_bin, csmith_include, timeout_compile)
    result["compile_a"] = {
        "rc": cc_a.returncode,
        "timeout": cc_a.timeout,
        "log": short(cc_a.stdout),
    }
    result["compile_b"] = {
        "rc": cc_b.returncode,
        "timeout": cc_b.timeout,
        "log": short(cc_b.stdout),
    }
    if cc_a.returncode != 0 or cc_a.timeout:
        result["status"] = "compile_a_failed"
        return result
    if cc_b.returncode != 0 or cc_b.timeout:
        result["status"] = "compile_b_failed"
        return result

    run_a = run_binary(a_bin, timeout_run)
    run_b = run_binary(b_bin, timeout_run)
    result["run_a"] = {
        "rc": run_a.returncode,
        "timeout": run_a.timeout,
        "out": run_a.stdout.strip(),
    }
    result["run_b"] = {
        "rc": run_b.returncode,
        "timeout": run_b.timeout,
        "out": run_b.stdout.strip(),
    }

    a_key = semantic_key(run_a)
    b_key = semantic_key(run_b)
    orig_key = semantic_key(run_orig)
    result["semantic_a"] = a_key
    result["semantic_b"] = b_key
    result["semantic_original"] = orig_key

    result["consistent"] = a_key == b_key
    result["a_matches_original"] = a_key == orig_key
    result["b_matches_original"] = b_key == orig_key
    result["status"] = "ok"
    return result


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare semantic behavior of outputs from two RetDec binaries on CSmith samples"
    )
    parser.add_argument(
        "--retdec-a", required=True, help="Path to original/baseline retdec-decompiler"
    )
    parser.add_argument(
        "--retdec-b", required=True, help="Path to modified/candidate retdec-decompiler"
    )
    parser.add_argument(
        "--csmith",
        default="/home/ruotoy/csmith/bin/csmith",
        help="Path to csmith binary",
    )
    parser.add_argument(
        "--csmith-include",
        default="/home/ruotoy/csmith/include",
        help="Path to csmith headers",
    )
    parser.add_argument("-n", "--count", type=int, default=20, help="Number of samples")
    parser.add_argument(
        "--start-seed",
        type=int,
        default=1,
        help="Start seed when --random-seeds is not used",
    )
    parser.add_argument("--random-seeds", action="store_true", help="Use random seeds")
    parser.add_argument(
        "--seed", type=int, default=1337, help="RNG seed used with --random-seeds"
    )
    parser.add_argument(
        "--workdir",
        default="/tmp/retdec-csmith-diff",
        help="Directory for all artifacts and reports",
    )
    parser.add_argument(
        "--timeout-gen", type=int, default=30, help="Timeout for csmith generation"
    )
    parser.add_argument(
        "--timeout-compile", type=int, default=30, help="Timeout for gcc compile"
    )
    parser.add_argument(
        "--timeout-decompile", type=int, default=180, help="Timeout for one retdec run"
    )
    parser.add_argument(
        "--timeout-run", type=int, default=10, help="Timeout for running binaries"
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    retdec_a = Path(args.retdec_a).resolve()
    retdec_b = Path(args.retdec_b).resolve()
    csmith_bin = Path(args.csmith).resolve()
    csmith_include = Path(args.csmith_include).resolve()
    workdir = Path(args.workdir).resolve()

    ensure_executable(retdec_a, "retdec-a")
    ensure_executable(retdec_b, "retdec-b")
    ensure_executable(csmith_bin, "csmith")
    if not csmith_include.exists():
        raise FileNotFoundError(f"csmith include directory not found: {csmith_include}")

    workdir.mkdir(parents=True, exist_ok=True)
    report_path = workdir / "report.json"

    if args.random_seeds:
        rng = random.Random(args.seed)
        seeds = [rng.randint(1, 2**31 - 1) for _ in range(args.count)]
    else:
        seeds = list(range(args.start_seed, args.start_seed + args.count))

    print(f"[info] retdec-a: {retdec_a}")
    print(f"[info] retdec-b: {retdec_b}")
    print(f"[info] count: {args.count}, workdir: {workdir}")
    print(f"[info] seeds: {seeds[:5]}{' ...' if len(seeds) > 5 else ''}")

    started = time.time()
    results: List[Dict[str, object]] = []
    for idx, seed in enumerate(seeds, start=1):
        case_dir = workdir / f"case_{idx:04d}_seed_{seed}"
        print(f"[run] {idx}/{len(seeds)} seed={seed}")
        res = process_one(
            seed=seed,
            case_dir=case_dir,
            csmith_bin=csmith_bin,
            csmith_include=csmith_include,
            retdec_a=retdec_a,
            retdec_b=retdec_b,
            timeout_gen=args.timeout_gen,
            timeout_compile=args.timeout_compile,
            timeout_decompile=args.timeout_decompile,
            timeout_run=args.timeout_run,
        )
        results.append(res)
        status = res["status"]
        consistent = res.get("consistent", False)
        print(f"      status={status} consistent={consistent}")

    metrics = {
        "total": len(results),
        "ok": sum(1 for r in results if r.get("status") == "ok"),
        "consistent_ok": sum(
            1 for r in results if r.get("status") == "ok" and r.get("consistent")
        ),
        "a_matches_original": sum(
            1
            for r in results
            if r.get("status") == "ok" and r.get("a_matches_original")
        ),
        "b_matches_original": sum(
            1
            for r in results
            if r.get("status") == "ok" and r.get("b_matches_original")
        ),
    }

    duration = round(time.time() - started, 2)
    report = {
        "command": " ".join(shlex.quote(x) for x in sys.argv),
        "retdec_a": str(retdec_a),
        "retdec_b": str(retdec_b),
        "csmith": str(csmith_bin),
        "csmith_include": str(csmith_include),
        "workdir": str(workdir),
        "duration_sec": duration,
        "metrics": metrics,
        "results": results,
    }
    report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")

    print("[done] report:", report_path)
    print("[done] duration_sec:", duration)
    print("[done] metrics:", json.dumps(metrics))
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as exc:
        print(f"[error] {exc}", file=sys.stderr)
        sys.exit(2)
