#!/bin/python3

from enum import Enum
import re
import semver
import subprocess
from typing import Dict
from typing import List

# TODO add option parsing
# TODO add option for skipping commit and tag
# TODO add option for ignoring dirty work directory
# TODO add option for specifying commit message format


VERSION_HEADER_FILE="./src/version.h"


class CommitType(Enum):
    MAJOR = 3
    MINOR = 2
    PATCH = 1
    def __lt__(self, other):
        if self.__class__ is other.__class__:
            return self.value < other.value
        return NotImplemented


def parse_git_log(output: str) -> List[Dict]:
    pattern = r"^(?P<hash>\w{7}) (?P<message>.*)$"
    lines = output.split('\n')
    commits = []

    for line in lines:
        match = re.match(pattern, line)
        if match:
            commits.append(match.groupdict())

    return commits


def get_last_tag() -> str:
    process_get_last_tag = subprocess.run(["git", "describe" ,"--tags", "--abbrev=0"], capture_output=True)
    return process_get_last_tag.stdout.decode('utf-8')[0:-1];


def get_commits_since_tag(tag: str) -> List[Dict]:
    process_commits_since = subprocess.run(["git", "log", "%s..HEAD" % tag, "--oneline"], capture_output=True)
    raw_commits_since = process_commits_since.stdout.decode('utf-8')[0:-1]

    return parse_git_log(raw_commits_since)


def create_git_tag(tag: str):
    subprocess.run(["git", "tag", tag])


def is_git_workdir_clean():
    process_status_porcelain = subprocess.run(["git", "status", "--porcelain"], capture_output=True)
    return len(process_status_porcelain.stdout.decode('utf-8')) == 0


def commit_header(version: str):
    subprocess.run(["git", "add", VERSION_HEADER_FILE])
    subprocess.run(["git", "commit", "--message", "chore: %s" % version], capture_output=True)


def determine_greatest_commit_type(commits: List[Dict]) -> CommitType:
    greatest_commit_type = CommitType.PATCH

    for commit in commits:
        # split by colon, commit should be in conventional commits format (without scope)
        message_components = commit['message'].split(':')
        commit_type = message_components[0]

        if "!" in commit_type:
            greatest_commit_type = CommitType.MAJOR
            # greatest possible commit type found, break
            break
        elif commit_type == "feat" and greatest_commit_type < CommitType.MINOR:
            greatest_commit_type = CommitType.MINOR

    return greatest_commit_type


def determine_version(last_tag: str, greatest_commit_type: CommitType) -> semver.Version:
    last_version = semver.Version.parse(last_tag)

    if greatest_commit_type == CommitType.MAJOR:
        return last_version.bump_major()
    if greatest_commit_type == CommitType.MINOR:
        return last_version.bump_minor()
    if greatest_commit_type == CommitType.PATCH:
        return last_version.bump_patch()


def write_version_to_headers(version: semver.Version):
    write_data = []
    
    with open(VERSION_HEADER_FILE, "r") as file:
        # this kind of repetitive, but it works well enough
        for line in file.readlines():
            if line.startswith("#define VERSION_MAJOR"):
                line = "#define VERSION_MAJOR \"%s\"" % version.major
            elif line.startswith("#define VERSION_MINOR"):
                line = "#define VERSION_MINOR \"%s\"" % version.minor
            elif line.startswith("#define VERSION_PATCH"):
                line = "#define VERSION_PATCH \"%s\"" % version.patch
            elif re.match(r"(//\\s*)?#define VERSION_PRERELEASE", line):
                if version.prerelease:
                    line = "// #define VERSION_PRERELEASE \"\""
                else:
                    line = "#define VERSION_PRERELEASE \"%s\"" % version.prerelease
            
            write_data.append(line if line[-1] != "\n" else line[0:-1])
    
    # add line break to end
    if write_data[-1] != "":
        write_data.append("")

    with open(VERSION_HEADER_FILE, "w") as file:
        file.write("\n".join([str(line) for line in write_data]))


if not is_git_workdir_clean():
    raise Exception("Git work directory is not clean")

last_tag    = get_last_tag()
commits     = get_commits_since_tag(last_tag)
commit_type = determine_greatest_commit_type(commits)
version     = determine_version(last_tag, commit_type)
version_str = str(version)

write_version_to_headers(version)
commit_header(version_str)
create_git_tag(version_str)
