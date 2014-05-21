#!/usr/bin/perl
use warnings;
use strict;

die "Usage: $0 fixedimage movingimage\n" if scalar @ARGV != 2;

my $fixedimage = $ARGV[0];
my $floatimage = $ARGV[1];
my $scale = 0;

my $fixedkey = $fixedimage . ".key";
my $floatkey = $floatimage . ".key";

die "$fixedimage doesn't exist\n" if not defined -e $fixedimage;
die "$floatimage doesn't exist\n" if not defined -e $floatimage;

`generatekeys $fixedimage $fixedkey $scale`;
`generatekeys $floatimage $floatkey $scale`;
`autopano --integer-coordinates alignment.pto $fixedkey $floatkey`;

open(my $landmarks, ">", "landmarks.ldm") or die "couldn't open landmarks.ldm\n";
open(my $alignment, "<", "alignment.pto") or die "couldn't open alignment.pto";

for (<$alignment>) {
  print $landmarks "$1 $2 $3 $4\n" if /x(\d+) y(\d+) X(\d+) Y(\d+)/;
}
