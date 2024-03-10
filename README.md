# 'Hello World!' as an ELF file
### Generates an executable ELF file that says "Hello, World!" for x86 or x86_64 Linux

## Building

To build, simply navigate to the directory containing the version you wish to build and run the build script.

For example, to build the x86 version written in C, you'd use

```$> cd C-i386 && ./C32-build.sh```

You may or may not have to give the script execute permissions with `chmod`.

The build script will ask you what name you wish to give the elf file. The default name is 'elf-file'.
