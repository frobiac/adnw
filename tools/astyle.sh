#!/bin/sh

# cp $0 /tmp/
# git filter-branch -f --tree-filter 'bash /tmp/astyle.sh ' -- HEAD~1..HEAD

# options from .astyle.rc
OPTIONS=" --style=linux --lineend=linux --indent=spaces=4 --convert-tabs --indent-preproc-block --indent-preproc-define --indent-switches --keep-one-line-blocks --keep-one-line-statements --suffix=none"

# astyle --options=.astyle.rc $(find src/ -name "*.[ch]")
astyle -q $OPTIONS $(find src/ -name "*.[ch]")

# trailing WS
sed -i "s/\s\s*$//" $(find . -name "*.sh" -o -name "*.md" -o -name "*.txt")
