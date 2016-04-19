#!/bin/bash

set -e

if [ ! -e jni/SDL ]; then
	echo "ERROR: The symlink \"./jni/SDL\" has to point to an SDL2 source tree"
	exit 1
fi

if [ -z "$ANDROID_HOME" ]; then
	echo "ANDROID_HOME is not set"
	exit 2
fi

if [ ! -e ${ANDROID_HOME}/tools/ant/build.xml ]; then
	echo "ANDROID_HOME does not point to an Android SDK installation, could not find ${ANDROID_HOME}/tools/ant/build.xml ."
	exit 3
fi

ndk-build clean

ndk-build -j4

ant debug
