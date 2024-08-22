# Mario Kart Wii - Service Pack

Mario Kart Wii - Service Pack is an experimental, open-source, cross-platform mod for MKW aiming to provide a variety of features and improvements over the base game. More details on the [website](https://mkw-sp.com).

## About this Branch

The goal of this branch is to add WWFC. I (hnery) made this branch for two reasons:

- WWFC (or Wiimmfi, but I choose WWFC) is a prerequisite to another project I am working on.
- I wanted to document and and be able to modify the online code.

Other than adding WWFC, I am also looking to add more documentation. This includes explaining non-obvious code, adding more types and functions from the base game (even if sp isn't using it), adding doxygen comments, etc.

I nuked a lot of sp's code; some of these changes could be seen as invasive. As a result, I don't plan to PR this into the main branch. Feel free to use this, but be careful to ensure your settings are compatible so you don't get banned from WWFC.

## Building

You need:

- devkitPPC (with the DEVKITPPC environment variable set)
- ninja (samurai also works)
- protoc
- Python 3
- pyjson5 (if installing from pip, the package is `json5` NOT `pyjson5`)
- pyelftools
- itanium\_demangler
- protobuf (the Python package)

Compile the project by running `build.py`:

```bash
./build.py
```

The `out` directory will contain the generated binaries and assets.

## Contributing

If you are working on something please comment on the relevant issue (or open a new one if necessary).

The codebase uses both C and asm, C should be preferred for full function replacements and for any kind of complex logic. No assumption about the use of registers by C code should be made other than the ABI. If necessary asm wrappers can be employed to restore and backup volatile registers.

The codebase is automatically formatted using `clang-format` (15), this will be checked by CI and must be run before merge.

If you need a unoptimised build with debugging information, use `python3 build.py -- debug`.

## Resources

- [Ghidra project](https://drive.google.com/drive/folders/1I1VRfeut3NtPeddePutfAaZhduVdKhhc?usp=sharing) (by far the most complete resource)

- [MKW decompilation](https://github.com/riidefi/mkw) (the most readable if it has what you need)

- [mkw-structures](https://github.com/SeekyCt/mkw-structures) (if you still haven't found)

- [Tockdom Wiki](http://wiki.tockdom.com/wiki/Main_Page) (file format documentation)
