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
git rebase --interactive master --exec " \
    bash /tmp/sizelog_one.sh > git.exec.size; \
    git notes add -f -F git.exec.size; \
"
