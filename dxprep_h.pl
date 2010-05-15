#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  dxprep_h.pl
#
#        USAGE:  ./dxprep_h.pl 
#
#  DESCRIPTION:  Prepare header files using Macro class notation for doxygen
#
#      OPTIONS:  ---
# REQUIREMENTS:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:  Felix Palmen, <felix@palmen-it.de>
#      COMPANY:  
#      VERSION:  1.0
#      CREATED:  15.05.2010 19:36:09 CEST
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;

open(HEADER, "<$ARGV[0]");
while(<HEADER>)
{
	if (/CLASS\((.+?)\)/)
	{
		print "public class $1 : ";
		do ($_ = <HEADER>) until (/INHERIT\((.+)\)/);
		print "$1\n{\n";
	}
	elsif (/(\s*)(.*)FUNC\((.+?)\)\s+ARG(.*)/)
	{
		print "$1public $2$3$4\n";
	}
	else
	{
		print;
	}
}
close(HEADER);

