#!/bin/bash
#FLAGS=" --linelength=120 --filter=+build/include_alpha,+build/include_what_you_use,+build/namespaces,-whitespace/indent,+blank_line,-runtime/references,-readability/streams,-whitespace/semicolon,-build/include,+build/include_alpha"
FLAGS="--linelength=120 --filter=-whitespace/indent"
find src -name "*.h" -or -name "*.cpp" | xargs python cpplint.py --root=src $FLAGS
