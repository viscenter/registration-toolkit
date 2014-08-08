#!/usr/bin/perl
use warnings;
use strict;
use File::Basename;

#Usage: 0 CSV File
#Takes the original (static) image and composes the difference with the 
#registered image 

while (<>) {
    my @commands = split ", ";
    if ($commands[3]) {
	my ($file, $dir, $ext) = fileparse($commands[3], qr/\.[^.]*/);
	my $out = "$dir"."diffs/$file" . "_diff$ext";
	system("composite $commands[1] $commands[3] -compose difference $out");
	system("convert $out -resize 20% $out");
    }
}
