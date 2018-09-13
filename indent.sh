#!/bin/sh
cd LunaDll
find \( -name "*.cpp" -or -name "*.h" \) -not -path "./libs/*" | xargs -I foo sh -c "echo foo && expand -i -t 4 foo > foo-t && mv foo-t foo"
