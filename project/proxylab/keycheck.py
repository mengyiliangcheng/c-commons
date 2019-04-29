#!/usr/bin/python

#
# keycheck - Script to check validity of key.txt
#

from __future__ import print_function, with_statement
import sys

# Open key.txt file
try:
    with open("key.txt") as keyfile:
        lines = keyfile.readlines()
except EnvironmentError as e:
    print("ERROR: Couldn't open key.txt file:", e)
    sys.exit(2)

# Extract lines
if len(lines) != 3:
    print("ERROR: key.txt file is not 3 lines long")
    sys.exit(2)

andrew_id = lines[0].strip()
github_id = lines[1].strip()
assess_me_key = lines[2].strip()

errors = []

# Check Andrew ID
if andrew_id.startswith('<'):
    errors.append("Please enter your Andrew ID in the first line of key.txt.")

# Check GitHub username
if github_id.startswith('<'):
    errors.append("Please enter your GitHub username in the second line of "
        "key.txt.")

# Check AssessMe key
if assess_me_key.startswith('<'):
    errors.append("Please enter your AssessMe key in the third line of "
        "key.txt.")

elif len(assess_me_key) != 32:
    errors.append("Key length is incorrect length. Please verify you have "
        "correctly entered the key you received from AssessMe. Follow the "
        "format specified in the writeup!")

# Print any errors
if errors:
    print("ERROR: key.txt file is invalid.")
    print('\n'.join(errors))
    sys.exit(1)
