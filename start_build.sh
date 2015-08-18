#!/bin/bash

gyp --depth . build.gyp

make -j5
