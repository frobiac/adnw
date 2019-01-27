#!/bin/sh
Y=$(date +%Y)

# add range if not current year
sed -i "s/ \(20[0-9][0-9]\) Stef/ \1-${Y} Stef/" $(grep -L " $Y Stef" $(git ls-files | grep -v LUFA ) )
# update range to current year
sed -i "s/-20[0-9][0-9] Stef/-${Y} Stef/" $(git ls-files | grep -v LUFA)

# these do have separate licenses
IGNORED="src/_private_data_template.h src/jump_bootloader.c src/jump_bootloader.h src/mem-check.c src/mem-check.h src/xprintf.h "

echo "# Files without any copyright (without ignored ones):"
MISSING=$(grep -L "[Cc]opyright " $(git ls-files "*.c" "*.h" | grep -v "LUFA/" | grep -v "src/external/"))

for i in $(echo $MISSING $IGNORED | tr ' ' '\n'| sort | uniq -u ); do
    printf "%-40s" $i
    git log --oneline --follow $i | tail -n1
done

#echo "# Files without my copyright:"
#grep -L "\-20[0-9][0-9] Stef" $(git ls-files "*.c" "*.h")
