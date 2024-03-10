#!/bin/sh

PNAME=hello-elf-i386
DEFAULT=i386-elf-file

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
    echo "Hit enter for default name ($DEFAULT)"
    read ELFNAME

    if [ "$ELFNAME" = "" ]; then
        ./${PNAME} ${DEFAULT}
    else
        ./${PNAME} ${ELFNAME}
    fi

    rm -f ${PNAME}
fi