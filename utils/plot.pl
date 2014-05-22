#!/usr/bin/perl
use warnings;
use strict;

open(my $data, ">", "data.dat");

while (<>) {
    print $data "$1 $2\n" if /(\d+)\s+(-\d+[.]\d+)/
}

system("gnuplot ~/visDev/registration-toolkit/utils/plot.in");
system("rm data.dat");
