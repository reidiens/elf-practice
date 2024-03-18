#!/bin/sh

PNAME=hello-elf-amd64
DEFAULT=amd64-elf-file
ENAME=""

function checkElfName() {       # arg1: the name to be checked. arg2: where the name will be stored
    if [ $(ls | grep -w $1) ]; then
        echo "WARNING! File \"$1\" exists!"
        echo "Do you wish to overwrite \"$1\"? (y/n)"
        read ANS
        echo ""

        if [ "$ANS" = "y" ] || [ "$ANS" = "Y" ]; then 
            eval "$2=$1"
        
        else 
            echo "What do you want to name the elf file?"
            echo "Hit enter for default name ($DEFAULT)"
            read ELFNAME
            echo ""

            if [ "$ELFNAME" != "" ]; then
                eval "checkElfName ${ELFNAME} $2"
            else 
                echo "Using default file name"
                echo ""
                eval "$2=${DEFAULT}"
                sleep 0.5 
            fi 

        fi
         
    else 
        eval "$2=$1"
    fi
}

echo "building..."
make PROG=${PNAME}
echo "built!"
echo ""

if [ "$1" != "" ]; then
    checkElfName $1 ENAME
    
else
    echo "What do you want to name the elf file?"
    echo "Hit enter for default name ($DEFAULT)"
    read ELFNAME
    echo ""
    
    if [ "$ELFNAME" = "" ]; then
        echo "Using default file name"
        echo ""
        ENAME=${DEFAULT}
        sleep 0.5

    else
        eval "checkElfName $ELFNAME ENAME"
    fi
fi

echo "generating elf file..."
./${PNAME} ${ENAME}
chmod +x ./${ENAME}
    
rm -f ${PNAME}