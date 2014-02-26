/ *******************************************************************************************

README File for CSIC 3753 Lab 2
Author: Christopher Jordan
Date: 02/26/2014
Description: README file for mutli-lookup.c, multi-lookup.h, and Makefile

******************************************************************************************* /

Building and running:

	- Normal Method:
    	Run "make" in main dir
    	Run "/.multi-lookup <input> <input> <output>"
        	<input> is a text input file with hostnames, one per line
        	<output> is a text file with hostname and IP addresses

	- Testing Method (runs Valgrind):
    	Run "make test-multi-lookup" in main dir
    		Removes existing "output.txt" file if in dir and autoruns program with testing
    		
    		This program looks to the "input" directory and loads all .txt file as input
    		Automatically outputs to "output.txt"

    - Auto-Run Method (loads input files and output automatically):
    	Run "make run-multi-lookup" in main dir
    		Removes existing "output.txt" file if in dir and autoruns program

    		This program looks to the "input" directory and loads all .txt file as input
    		Automatically outputs to "output.txt"

    - Cleaning Everything:
    	Run "make clean"
    		Removes compiled object files and any written output.txt files

    - Cleaning just output:
    	Run "make clean-output"
    		Removes only written "output.txt" file from the dir
