#!/bin/bash

set -e

ndk-build -j4

ant debug

