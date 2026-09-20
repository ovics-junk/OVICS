# OVICS

**Open World Interface Computer System**

OVICS is a lightweight hobby operating system built from scratch.

## Version

Current version: **0.0.2**

## Current features
- Custom 32-bit x86 kernel
- VGA text mode
- PS/2 keyboard input
- Command-line interface
- Commands: help, version, clear, echo

## Architecture

OVICS currently runs as a **32-bit i386 operating system**.

Boot process:

Multiboot → boot.asm → OVICS kernel

## Status

OVICS is currently in a very early stage of development.

Version 0.0.2 is the first publicly released version.

## Goal

The goal of OVICS is to create a lightweight operating system that can eventually run on older computers and tablets.

## Development

Built using C and Assembly.

Currently tested with QEMU.

## Warning

OVICS is experimental software.
It is not ready for everyday use.