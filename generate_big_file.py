import time
from pathlib import Path

import numpy as np
DEST_FOLDER = Path("E:\\tmp")
OUTPUT = DEST_FOLDER / "big.bin"
TOTAL_BYTES = 6 * 1024**3  # 30 GiB
CHUNK_BYTES = 64 * 1024**2  # 64 MiB


def main() -> None:
    rng = np.random.default_rng()
    start = time.monotonic()

    with open(OUTPUT, "wb") as f:
        written = 0
        while written < TOTAL_BYTES:
            n = min(CHUNK_BYTES, TOTAL_BYTES - written)
            f.write(rng.bytes(n))
            written += n

    elapsed = time.monotonic() - start
    written_mb = written / (1024 * 1024)
    mb_per_s = written_mb / elapsed
    print(f"Wrote {written:,} bytes to {OUTPUT} in {elapsed:.1f}s ({mb_per_s:.1f} MB/s)")


if __name__ == "__main__":
    main()
