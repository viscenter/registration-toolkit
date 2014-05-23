#!/usr/bin/perl
use warnings;
use strict;
use File::Find;

my $iterations = 100;
my $reg_suffix = "_Reg2010.tif";
my $tfm_suffix = "_Reg2010.tfm";
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

my %transforms;
for (@landmarks) {
  /(.+)landmarks/;
  $transforms{$1 . "transforms"}++
}

for (keys %transforms) {
  mkdir $_ if not defined -e $_
}


open(my $csv, ">", "registration_batch.csv");

for my $ldm (@landmarks) {
  $ldm =~ /\d+-(.+)-(\d{4})/;
  my ($page, $year) = ($1, $2);
  my $output_image = $data{$page}->{$year};
  $output_image =~ s/[.]tif$/$reg_suffix/i;

  my $tfm_file = $ldm;
  $tfm_file =~ s/[.]ldm$/$tfm_suffix/i;
  $tfm_file =~ s/landmarks/transforms/;

  print $csv "$ldm, $data{$page}->{2010}, $data{$page}->{$year}, $output_image, $tfm_file, $iterations\n";
}
