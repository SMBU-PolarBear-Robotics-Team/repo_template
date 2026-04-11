#!/bin/bash
set -e

# Default build type
BUILD_TYPE=${BUILD_TYPE:-RelWithDebInfo}
# Default packages if no argument
DEFAULT_PACKAGES="repo_template"

# Determine package filter
# Usage:
#   ./build.sh          -> build DEFAULT_PACKAGES (and their deps)
#   ./build.sh all      -> build entire workspace
#   ./build.sh pkg1 ... -> build specified packages (and their deps)
if [ "$#" -eq 0 ]; then
  PACKAGES_ARGS="--packages-up-to $DEFAULT_PACKAGES"
elif [ "$1" = "all" ]; then
  PACKAGES_ARGS=""
else
  PACKAGES_ARGS="--packages-up-to $@"
fi

colcon build \
  --merge-install \
  --symlink-install \
  --cmake-args "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" \
  --parallel-workers 10 \
  $PACKAGES_ARGS
