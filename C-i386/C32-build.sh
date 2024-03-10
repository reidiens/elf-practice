#!/bin/sh

if [ "$1" = "" ]; then
    PNAME="hello-elf"    
else
    PNAME="$1"
fi

echo "building..."
make PROG=${PNAME}
echo "built!"
chmod +x ${PNAME}

if [ "$1" != "" ] && [ "$2" != "" ]; then
    echo "generating ELF file..."
    ./${PNAME} "$2"
    rm -f ${PNAME}
else
    echo ""
    echo "What do you want the ELF file name to be?"
    echo "Hit enter for default name (elf-file)"
    read ELFNAME
    ./${PNAME} ${ELFNAME}
    rm -f ${PNAME}
fi