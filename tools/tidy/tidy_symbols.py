#!/usr/bin/env python3

'''
Usage: From the repository root, run:
    python tools/tidy/tidy_symbols.py

This script sorts all symbols by ascending address and normalizes addresses to lowercase.
'''

def parse_symbol(line):
    line = line.strip()
    if not line or line.startswith("#"):
        return None
    parts = line.split(maxsplit=1)
    if len(parts) != 2:
        return None
    addr_str, name = parts
    try:
        addr = int(addr_str, 16)
    except ValueError:
        return None
    return addr, name

def main():
    symbols_file = "symbols.txt"

    raw_lines = []
    symbols = []

    with open(symbols_file, "r") as f:
        for line in f:
            raw_lines.append(line.rstrip("\n"))
            parsed = parse_symbol(line)
            if parsed:
                symbols.append(parsed)

    symbols.sort(key=lambda x: x[0])

    sym_iter = iter(symbols)

    with open(symbols_file, "w") as f:
        for line in raw_lines:
            stripped = line.strip()

            if stripped == "" or stripped.startswith("#"):
                f.write(line + "\n")
                continue

            parsed = parse_symbol(line)
            if parsed:
                addr, name = next(sym_iter)
                f.write(f"0x{addr:08x} {name}\n")
            else:
                f.write(line + "\n")

if __name__ == "__main__":
    main()