#!/usr/bin/perl
use warnings;
use strict;
use File::Find;

my $iterations = 100;
my $output_suffix = "_Reg2010.tif";
our @landmarks;
our %data;

find(\&wanted, '.');
sub wanted {
  if (/[.]ldm$/) {
    push @landmarks, $File::Find::name
  } elsif (/\d+-(.+)-(\d{4})[.]tif$/i) {
    $data{$1}->{$2} = $File::Find::name
  }
}

open(my $csv, ">", "registration_batch.csv");
for my $ldm (@landmarks) {
  $ldm =~ /\d+-(.+)-(\d{4})/;
  my ($page, $year) = ($1, $2);
  my $output_image = $data{$page}->{$year};
  $output_image =~ s/[.]tif$/$output_suffix/i;
  print $csv "$ldm, $data{$page}->{2010}, $data{$page}->{$year}, $output_image, $iterations\n";
}
