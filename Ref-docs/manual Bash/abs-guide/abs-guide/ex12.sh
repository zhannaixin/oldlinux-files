#!/bin/bash

filename=sys.log

cat /dev/null > $filename; echo "Creating / cleaning out file."
# Creates file if it does not already exist,
# and truncates it to zero length if it does.
# : > filename   also works.

tail /var/log/messages > $filename  
# /var/log/messages must have world read permission for this to work.

echo "$filename contains tail end of system log."

exit 0
