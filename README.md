# test_py_fcopy

Benchmark large-file I/O in Python vs C on Windows.

Two pairs of equivalent scripts:

| Step     | Python                  | C                      |
|----------|-------------------------|------------------------|
| Generate | `generate_big_file.py`  | `generate_big_file.c`  |
| Copy     | `copy_big_file.py`      | `copy_big_file.c`      |

**Generate** writes a 6 GiB file (`E:\tmp\big.bin`) in 64 MiB chunks. A single random-filled buffer is reused for every write — this isolates raw write throughput from PRNG cost.

**Copy** reads `E:\tmp\big.bin` 64 MiB at a time and writes the same bytes to `E:\tmp\big_out.bin`. Read and write times are accumulated separately so you can see which side dominates.

## Requirements

- Windows
- Python 3.14.2 with `numpy` (`pip install numpy`)
- MSVC `cl.exe` (Visual Studio Build Tools or the IDE)
- `E:\tmp` directory must exist; ~12 GiB free on `E:` (6 GiB input + 6 GiB output)

## Run the Python versions

```powershell
python generate_big_file.py
python copy_big_file.py
```

## Compile and run the C versions with `cl`

`cl.exe` only resolves correctly inside an MSVC environment. Open **x64 Native Tools Command Prompt for VS** (start menu), then `cd` to this directory and:

```powershell
cl /O2 /nologo generate_big_file.c
.\generate_big_file.exe

cl /O2 /nologo copy_big_file.c
.\copy_big_file.exe
```

Flags:
- `/O2` — full speed optimization. Without it the PRNG and copy loops are far slower than they need to be and you measure the compiler, not the disk.
- `/nologo` — suppress the MSVC banner.

If you'd rather not open a VS prompt, you can initialize the env in PowerShell once per session:

```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

(Adjust the path for your VS edition — `Community`, `Professional`, `BuildTools`, etc.)

## Interpreting results

Both copy programs print three numbers: total elapsed, read MB/s, write MB/s.

Two things make the numbers slippery and worth knowing about:

1. **OS page cache.** After generating `big.bin`, it sits in Windows' file cache. The first copy run will read mostly from RAM, not disk, and writes return as soon as they hit write-back cache. Run each program **twice** and compare — or reboot for a cold-cache number.
2. **Timer resolution in C.** `clock()` has 1 ms resolution on Windows. With 64 MiB chunks that's fine; if you shrink `CHUNK_BYTES` significantly, switch to `QueryPerformanceCounter` for accurate per-chunk numbers.

## Layout

```
generate_big_file.py   # write 6 GiB random data to E:\tmp\big.bin
generate_big_file.c    # same, in C
copy_big_file.py       # read big.bin, write big_out.bin, time read & write
copy_big_file.c        # same, in C
CLAUDE.md              # behavioral guidelines for the AI assistant
```
