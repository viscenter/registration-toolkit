#!/usr/bin/perl
use warnings;
use strict;
use File::Find;

my $iterations = 100;
my $output_suffix = "_Reg2010.jpg";
our @landmarks;
our %data;

find(\&wanted, '.');
sub wanted {
  if (/[.]ldm$/) {
    push @landmarks, $File::Find::name
  } elsif (/\d+-(.+)-(\d{4})[.]jpg$/) {
    $data{$1}->{$2} = $File::Find::name
  }
}

open(my $csv, ">", "registration_batch.csv");

for my $ldm (@landmarks) {
  $ldm =~ /\d+-(.+)-(\d{4})/;
  my ($page, $year) = ($1, $2);
  my $output_image = $data{$page}->{$year};
  $output_image =~ s/[.]jpg$/$output_suffix/;
  print $csv "$ldm, $data{$page}->{2010}, $data{$page}->{$year}, $output_image, $iterations\n";
}
