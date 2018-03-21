#!/bin/bash
find . -name "*.cc" -exec ./indent.bash {} \;
find . -name "*.h" -exec ./indent.bash {} \;
