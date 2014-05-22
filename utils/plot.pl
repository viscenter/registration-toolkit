#!/usr/bin/perl
use warnings;
use strict;
use File::Basename;

my $dirname = dirname(__FILE__);

open(my $data, ">", "data.dat");

while (<>) {
    print $data "$1 $2\n" if /(\d+)\s+(-\d+[.]\d+)/
}

system("gnuplot $dirname/plot.in");
system("rm data.dat");
