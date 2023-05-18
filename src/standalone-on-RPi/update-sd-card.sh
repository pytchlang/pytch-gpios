#!/bin/bash

set -e

# "Highlight"
H="$(echo -e '\033[1;33m')"

# "Normal"
N="$(echo -e '\033[0m')"

nginxconftemplate="$(dirname "$0")/pytch.conf"
if ! [ -r "$nginxconftemplate" ]; then
    >&2 echo "Could not read nginx conf template $H$nginxconftemplate$N"
    exit 1
fi

if [ -z "$1" ] || [ -z "$2" ]; then
    >&2 echo Usage: "$(basename "$0")": BUILD-ZIPFILE MOUNT-POINT-DIRECTORY
    exit 1
fi

if ! [ -r "$1" ]; then
    >&2 echo "Build zipfile $H$1$N not readable"
    exit 1
fi

if ! [ -d "$2" ]; then
    >&2 echo "Mount point $H$2$N is not a directory"
    exit 1
fi

buildzip="$1"
destdir="$2"

t="${buildzip#*beta-}"
buildid="${t%.zip}"

hname="$(cat "$destdir"/etc/hostname)"

echo "Working with build-id $H$buildid$N"
echo "Reading Nginx config template $H$nginxconftemplate$N"
echo "Writing to $H$destdir$N which has hostname $H$hname$N"

contentdir="$destdir"/var/www/html
nginxconffile="$destdir/etc/nginx/sites-available/pytch.conf"

if ! [ -w "$contentdir" ]; then
    >&2 echo "Cannot write to $contentdir; is sudo needed?"
    exit 1
fi

if ! [ -w "$nginxconffile" ]; then
    >&2 echo "Cannot write to $nginxconffile; is sudo needed?"
    exit 1
fi

echo -n "Unzipping to $contentdir... "
unzip -q "$buildzip" -d "$contentdir"
echo done

echo -n "Writing $nginxconffile... "
sed "s/@@BUILDID@@/$buildid/" < "$nginxconftemplate" > "$nginxconffile"
echo done

sync

echo
echo "All done"
