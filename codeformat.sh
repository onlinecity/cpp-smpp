#!/bin/bash
#FLAGS=""
find src -name "*.h" -or -name "*.cpp" | xargs astyle
#find test -name "*.h" -or -name "*.cpp" | xargs astyle

