# daylog-c
Rewrite of my simple time tracking CLI program in C.

## Dependencies

### Required

- bash
- gcc
- pcre2
- python3

### Optional dependencies
- [python-semver](https://pypi.org/project/semver/)
    - Needed for release script
    - Can be installed from the package `python-semver` on Arch Linux.
- [mypy](https://mypy-lang.org/)
    - Used for type checking Python scripts
    - Can be installed from the package `mypy` on Arch Linux.

## Getting started

Build project:

    ./scripts/build.sh

Run binary:

    ./build/daylog

Create release (increments version based on commits with Conventionals Commits spec)

    ./create-release.py

