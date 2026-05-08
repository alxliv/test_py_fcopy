import time
from pathlib import Path

DEST_FOLDER = Path("E:\\tmp")
INPUT = DEST_FOLDER / "big.bin"
OUTPUT = DEST_FOLDER / "big_out.bin"
CHUNK_BYTES = 64 * 1024**2  # 64 MiB


def main() -> None:
    buf = bytearray(CHUNK_BYTES)
    mv = memoryview(buf)
    read_time = 0.0
    write_time = 0.0
    total = 0

    start = time.monotonic()
    with open(INPUT, "rb") as f_in, open(OUTPUT, "wb") as f_out:
        while True:
            t = time.monotonic()
            n = f_in.readinto(buf)
            read_time += time.monotonic() - t
            if n == 0:
                break
            t = time.monotonic()
            f_out.write(mv[:n])
            write_time += time.monotonic() - t
            total += n

    elapsed = time.monotonic() - start
    total_mb = total / (1024 * 1024)
    print(f"Copied {total:,} bytes in {elapsed:.2f}s")
    print(f"  read:  {read_time:.2f}s ({total_mb / read_time:.1f} MB/s)")
    print(f"  write: {write_time:.2f}s ({total_mb / write_time:.1f} MB/s)")


if __name__ == "__main__":
    main()
