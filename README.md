![hemerodrome logo](pheidippides.png)

# Hemerodrome
Hemerodrome is a small, lightweight, process runner for Windows and POSIX platforms.
It supports running new processes (as a child, or replacing the caller), 
returning a PID (Process ID), waiting for termination (with a timeout) and forced termination.

It does not support advanced features such as streaming stdout or other forms of interprocess communication.
For a fully fledged process runner, see something like [Boost.Process](https://www.boost.org/doc/libs/1_72_0/doc/html/process.html).

## What does the name mean?

A Hemerodrome was an ancient Greek dispatch runner, such as Pheidippides, 
whose legendary run to Athens from the Battle of Marathon to deliver news of victory,
gave name to the modern long distance race.

Pheidippides, and others like him, were "pro" runners of their day,
so Hemerodrome seemed a fitting name. Plus it sounds mysterious and slightly cool.

## Status

This project is just getting started.
The Windows part hasn't been written at all yet - and the interface will likely change.

So feel free to try it out - but don't use it in any production code (or even long lasting toy code) yet. 
 
