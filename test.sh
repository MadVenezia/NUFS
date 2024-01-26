#!/usr/bin/env bash

mkdir mnt/foo
mkdir mnt/tmp 

echo -e "ls for basic directories\n"
ls mnt

echo -e "make nested dir1\n"

mkdir mnt/foo/bar

echo -e "full ls\n"

ls -R mnt

echo -e "deepnest1\n"

mkdir mnt/foo/bar/baz

echo -e "full ls\n" 

ls -R mnt

echo -e "\n\nthis is homophobic\n"

make unmount && make clean
