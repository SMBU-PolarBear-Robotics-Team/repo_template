#!/bin/bash
set -e

# Default packages if no argument
DEFAULT_PACKAGES="repo_template"

# Determine package filter
# Usage:
#   ./test.sh           -> test DEFAULT_PACKAGES
#   ./test.sh all       -> test entire workspace
#   ./test.sh pkg1 ...  -> test specified packages
if [ "$#" -eq 0 ]; then
  PACKAGES_ARGS="--packages-select $DEFAULT_PACKAGES"
elif [ "$1" = "all" ]; then
  PACKAGES_ARGS=""
else
  PACKAGES_ARGS="--packages-select $@"
fi

if [ -f install/setup.bash ]; then source install/setup.bash; fi

colcon test \
  --merge-install \
  --parallel-workers 10 \
  $PACKAGES_ARGS
colcon test-result --all --verbose
