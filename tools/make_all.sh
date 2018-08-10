#!/bin/bash

git submodule update
test -f LUFA-scrollwheel.patch && patch -Np1 -i LUFA-scrollwheel.patch
cp src/_priv*templat* src/private_data.h 2>&1

make clean
make KB_DBG=1 KB_HW="REDTILT" || exit 1

(echo -n 'Size: '; make size | grep bytes | sed 's/^\(.\).*: *\([0-9]*\) byt.*/\1 \2 /' | tr -d '\n' ; git describe) > git.exec.size
git notes add -f -F git.exec.size



exit

N=90
git co -b sizelog -f
git rbi master --exec "git submodule update; cp src/_priv*templat* src/private_data.h ; \
    (make clean && make KB_DBG=1 KB_HW=REDTILT) >/dev/null | exit 1; \
    (echo -n 'Size: '; make size | grep bytes | sed 's/^\(.\).*: *\([0-9]*\) byt.*/\1 \2 /' | tr -d '\n' ; git describe) > git.exec.size ;\
    git notes add -f -F git.exec.size; \
"
git rb --abort
git co -

exit

#
# one line size and description
make size | grep bytes | sed 's/^\(.\).*: *\([0-9]*\) byt.*/\1 \2 /' | tr -d '\n' ; git  describe

(make clean && make) >/dev/null;
    (echo -n 'Size: '; make size | grep bytes | sed 's/^\(.\).*: *\([0-9]*\) byt.*/\1 \2 /' | tr -d '\n' ; git describe) > git.exec.size
    git notes -a -f -F git.exec.size;
