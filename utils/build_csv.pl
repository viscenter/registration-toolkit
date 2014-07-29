#!/usr/bin/perl
use warnings;
use strict;
use File::Find;

#setting iterations and suffix
my $iterations = 400;
my $reg_suffix = "_Reg2010.tif";
my $tfm_suffix = ".tfm";

#declaring data structures
our @landmarks;
our %data;

#finding files 
find(\&wanted, '.');

#adding landmarks to list and files/data to hash
sub wanted {
  if (/[.]ldm$/) {
    push @landmarks, $File::Find::name
  } 
  elsif(/\d-(.+)-(.+)[.]tif$/){
    $data{$1}->{$2} = $File::Find::name;
  }
}

#making folders
my %folder;
for (@landmarks) {
  /(.+)landmarks/;
  $folder{$1 . "transforms"}++;
  $folder{$1 . "registered"}++
}

for (keys %folder) {
  mkdir $_ if not defined -e $_
}

#open csv to write to
open(my $csv, ">", "registration_batch.csv");

for my $ldm (@landmarks) {
  $ldm =~ /\d+-(.+)-(.+)_/;
  #Add appropriate images for the landmark
  my ($page, $year) = ($1, $2);
  my $output_image = $data{$page}->{$year};

  #create the output image name/folder
  $output_image =~ s/[.]tif$/$reg_suffix/i;
  $output_image =~ s/$year/$year\/registered/;
  
  #create the tfm file name 
  my $tfm_file = $ldm;
  $tfm_file =~ s/[.]ldm$/$tfm_suffix/i;
  $tfm_file =~ s/landmarks/transforms/;
  
  #write file
  print $csv "$ldm, $data{$page}->{2010}, $data{$page}->{$year}, $output_image, $tfm_file," . ' ' . "$iterations\n";
}
