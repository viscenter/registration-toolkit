#!/usr/bin/perl
use warnings;
use strict;
use File::Find;
use File::Find::Rule;
use Image::Magick;

my %data;
my $root = '.';
my @subdirs ;
my @files;

my ($hd1, $wd1, $hd2, $wd2);  #height and width difference

open(my $csv, ">", "auto_batch.csv");
#Will write csv in the format image1, wd, hd, image2, wd, hd

opendir my($dh), $root or die "Couldn't open current directory";
#if(grep { !/^\./ }) {# readdir $dh;
my @dirs =File::Find::Rule->new
  ->directory
  ->maxdepth('1')
  ->in($root);

shift @dirs;

for my $directs(@dirs){
  my @temp = File::Find::Rule->new
    ->directory
    ->mindepth('1')
    ->in($directs);
  push(@subdirs, @temp);
}

for my $folder(@dirs){
  my @temp = File::Find::Rule->new
    ->name('*.jpg')
    ->maxdepth('1')
    ->in($folder);
  push(@files, @temp);
}


for my $file(@files){
  (my $mainfolder, my $filename) = split(/\//, $file); 
  my ($manuscript, $id, $page, $year) = split(/-/,$ filename);  
  ($year, my $ext) = split(/\./, $year);
  my $info = ($page. '-' . $year);
  $data{$info} = $file;
}

my @pre2010 = File::Find::Rule->new
    ->name('*.jpg')
    ->maxdepth('1')
    ->in($dirs[0]);


for my $file(@pre2010){
  #Name of 1st image
  my($fname, $ext) = split(/\./, $file);
  (my $mainfolder, my $filename) = split(/\//, $fname);
  my ($manuscript, $id, $page, $year) = split(/-/, $filename);
  
  #Info for 1st image
  my $key = ("$page" . '-' . $dirs[1]);
  if(exists($data{$key})){
    print $csv $data{$key};
    print $csv ", ";

    #Getting width and height difference
    my $orig_name = $data{$key};
    my $i1 = Image::Magick->new;
    my $x1 = $i1->Read($orig_name);
    my $orig_height = $i1->Get('height');
    my $orig_width = $i1->Get('width');
 
    my($name10, $ext) = split(/\./, $orig_name);
    (my $mainfolder, my $filename) = split(/\//, $name10); 
    my ($manuscript, $id, $page, $year) = split(/-/, $filename); 
    my $tilename = "$mainfolder/$filename/$filename". "_tile1.jpg";
    
    my $i2 = Image::Magick->new;
    my $x2 = $i2->Read($tilename);
    my $tile_height = $i2->Get('height');
    my $tile_width = $i2->Get('width');

    $hd1 = $orig_height - $tile_height;
    $wd1 = $orig_width - $tile_width;
    print $csv $wd1, ", ", $hd1, ", ";
  }

  #Info for 2nd image
  $key = ("$page" . '-' . $dirs[0]); 
  #Name of 2nd image
  if(exists($data{$key})){
    print $csv $data{$key};
    print $csv ", ";

    #Getting width and height difference
    my $orig_name = $data{$key};
    my $i1 = Image::Magick->new;
    my $x1 = $i1->Read($orig_name);
    my $orig_height = $i1->Get('height');
    my $orig_width = $i1->Get('width');

    my $tilename = "$mainfolder/$filename/$filename". "_tile1.jpg";
    my $i2 = Image::Magick->new;
    my $x2 = $i2->Read($tilename);
    my $tile_height = $i2->Get('height');
    my $tile_width = $i2->Get('width');

    $hd2 = $orig_height - $tile_height;
    $wd2 = $orig_width - $tile_width;
    print $csv $wd2, ", ", $hd2, "\n ";

  }
}
