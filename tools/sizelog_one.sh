#!/bin/bash

mopts_line() {
    grep "^KB_" makefile | sed "s/ .*//" | sort | uniq | grep -v KB_HW | sed "s/\s*$/=0 /" | tr -d '\n'
    echo
}

CNT=$(mopts_line | wc -w )

mopts_line_all() {
    mopts_line | sed "s/0/1/g"
    mopts_line 0

    for i in $(seq 1 $CNT); do
        mopts_line 0 | sed "s/0/1/$i";
    done
}


# setup rebased dir
git submodule update -f
test -f LUFA-scrollwheel.patch && patch -Np1 -i LUFA-scrollwheel.patch
test -f src/_priv*templat* && cp src/_priv*templat* src/private_data.h

# Test standard build first.
(make clean && make -j 4) &>/dev/null || { echo "ERROR $(git describe) does not build"; exit 1; }


# header
echo "$(git describe) $(mopts_line ' ' ' ' ' ' ' ')"
echo ". . . .   : text data bss dec hex filename"
echo "Make      :" $(avr-size --mcu=atmega32u4 -B adnw.elf | sed "s/ text.*filename//" )

OLD=$IFS
IFS="
"
for opts in $(mopts_line_all); do
    IFS=$OLD
    opts_sel=$(echo $opts | tr -dc "[0-9 ]")
    (make clean && make -j 4 KB_HW=REDTILT ${opts} ) &>/dev/null || { echo "$opts_sel : ERROR" ; continue; }

    #  1 1 1 1 : text data bss dec hex filename 20164 464 416 21044 5234 adnw.elf
    echo $opts_sel : $(avr-size --mcu=atmega32u4 -B adnw.elf | sed "s/ text.*filename//" )
done

