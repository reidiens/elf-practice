# 'Hello World!' as an ELF file
### Generates an executable ELF file that says "Hello, World!" for x86 or x86_64 Linux

## Building

To build, simply navigate to the directory containing the version you wish to build and run the build script.

For example, to build the x86 version written in C, you'd use

```$> cd C-i386 && ./build.sh```

You may or may not have to give the script execute permissions with `chmod`.

You can pass the name you wish to give the generated ELF file as an argument to the build script. Ex:

```$> ./build.sh elf-file-name```

If the no name is passed, the build script will simply prompt the you to input a name. If nothing is entered, the default name is used.
