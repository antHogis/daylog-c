# daylog-c
Rewrite of my simple time tracking CLI program in C.

## Usage

### Defining the daylog file

By default, daylog looks for the daylog file at `$HOME/notes/day.log`.
Here you can define how you spent time on a given day. Example of a daylog entry

    ---- 22.11 ----------------------------------------
    0900-0930 PR
    0930-1000 PGS-732
    1000-1045 meeting
    1045-1330 PGS-732
    1330-1430 meeting
    1430-1630 PGS-732

Syntax for a line in the entry is
    
    <TIME_BEGIN>-<TIME_END> <TASK_ID OR DESCRIPTION>

The times are written in the `HHmm` format

Important to note
- The definition of the date must be exactly in the format given.
- The dates must be written in the `MM.DD` format. So January 2nd would be `02.01`.
- You can go over to the next day by adding the time to 2400
    - `2330-2530` results in 2 hours counted
    - This is still counted toward the day of the daylog entry
- Anything that's not recognized as a task id will be summed with the "other"
  description when outputting as csv

### Using daylog

Get the daylog summary of the current day

    daylog

Should output something like
    
    PR		    0h30m
    PGS-732		5h15m
    meeting		1h45m
    -------------------------------------------
    TOTAL 7h30m

You can also specify the date

    daylog 2024-11-22

If you want to output the summaries of a date span as csv, you can do this

    daylog -c -s 2024-11-01 -e 2024-11-31


## Dependencies

### Required

- bash
- gcc
- pcre2

### Optional dependencies
- python3
    - Needed for release script
- [python-semver](https://pypi.org/project/semver/)
    - Needed for release script
    - Can be installed from the package `python-semver` on Arch Linux.
- [mypy](https://mypy-lang.org/)
    - Used for type checking Python scripts
    - Can be installed from the package `mypy` on Arch Linux.



## Building and running

Build project:

    ./scripts/build.sh

Run binary:

    ./build/daylog

Create release (increments version based on commits with Conventionals Commits spec)

    ./create-release.py

