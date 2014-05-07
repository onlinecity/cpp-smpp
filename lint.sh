#!/bin/bash
FLAGS="--linelength=120 --filter=+,-build/header_guard"
find src -name "*.h" -or -name "*.cpp" | xargs cpplint.py --root=src $FLAGS
find test -name "*.h" -or -name "*.cpp" | xargs cpplint.py --root=test $FLAGS
