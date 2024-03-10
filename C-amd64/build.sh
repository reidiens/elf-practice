#!/bin/sh

PNAME=hello-elf-amd64
DEFAULT=amd64-elf-file

echo "building..."
make PROG=${PNAME}
echo "built!"

if [ "$1" != "" ]; then
    echo "generating elf file..."
    ./${PNAME} "$1"
    chmod +x ./"$1"

else
    echo ""
    echo "What do you want to name the elf file?"
    echo "Hit enter for default name ($DEFAULT)"
    read ELFNAME

    if [ "$ELFNAME" = "" ]; then
        echo "File will be created with default name"
        sleep 1
        echo "generating elf file..."
        ./${PNAME} ${DEFAULT}
        chmod +x ${DEFAULT}
    
    else 
        echo "generating elf file..."
        ./${PNAME} ${ELFNAME}
        chmod +x ./${ELFNAME}

    fi
fi

rm -f ${PNAME}