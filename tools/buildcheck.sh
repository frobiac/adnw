#!/bin/bash
N=2

TMP=$$

cleanup()
{
    git rebase --abort
    git checkout - && git branch -D sizelog-${TMP}
}

cp tools/sizelog_one.sh /tmp/

trap cleanup EXIT

git checkout -b sizelog-${TMP} -f
#git rebase --interactive 1a4f13ed8fd8d05102fe5ed3f745e7e438c89933 --exec " \
OPT="KB_HW=REDTILT KB_DBG=0 "

echo "# $OPT $(date) " >> err.log

git rebase --interactive master --exec " \
    (git submodule update -f ; \
    test -f LUFA-scrollwheel.patch && patch -Np1 -i LUFA-scrollwheel.patch; \
    test -f src/_priv*templat* && cp src/_priv*templat* src/private_data.h; \
    ) >/dev/null 2>&1; \
    (make clean; make -j 8 $OPT ) >/dev/null 2>&1 || echo \$(git describe) ERROR >> err.log ; \
"
