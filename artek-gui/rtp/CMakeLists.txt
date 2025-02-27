cmake_minimum_required(VERSION 3.5)
project(rtp LANGUAGES C)

add_library(rtp STATIC rtp-packer.c rtp-unpacker.c util.c common.c)
