/*
 * File: multi-lookup.c
 * Author: Christopher Jordan
 * Project: CSCI 3753 Programming Assignment 2
 * Create Date: 2014/02/22
 * Modify Date: 2014/02/22
 * Description: Program allowing multi-threaded lookup for domain name to IP addresses
 */

#include "multi-lookup.h"
#include "queue.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MINARGS 3
#define USAGE "<inputFilePath> <outputFilePath>"
#define SBUFSIZE 1025
#define INPUTFS "%1024s"