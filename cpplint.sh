#!/bin/bash
FLAGS="--linelength=120 --filter=-whitespace/indent"
find src -name "*.h" -or -name "*.cpp" | xargs python cpplint.py --root=src $FLAGS
find test -name "*.h" -or -name "*.cpp" | xargs python cpplint.py --root=test $FLAGS
