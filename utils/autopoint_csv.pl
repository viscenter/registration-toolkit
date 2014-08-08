#!/usr/bin/perl
use warnings;
use strict;
use File::Find;
use File::Find::Rule;
use Image::Magick;

#This program generates a CSV file to use with the auto_align_tiles program.
#It finds .jpg files and matches pages based on the name Manuscript-OrderNumber-PageNumber-Year.ext

my %data;
my $root = '.';
my @subdirs ;
my @files;
my @moving;

my ($hd1, $wd1, $hd2, $wd2);  #height and width difference

open(my $csv, ">", "auto_batch.csv");
#Will write csv in the format image1, wd, hd, image2, wd, hd

opendir my($dh), $root or die "Couldn't open current directory";

#Find files and directories 
@files = File::Find::Rule->new
  ->name('*.jpg')
  ->maxdepth('2')
  ->in($root);

my @dirs =File::Find::Rule->new
  ->directory
  ->maxdepth('1')
  ->in($root);

my $answer = "t";

while(($answer !~ 'm') and ($answer !~ 's')){
  print("Is $dirs[1] the [m]oving set or the [s]tatic set?  ");
  $answer = <>; 
}
#Split and add image data to hash
#The key is the pagenumber-year, the value is the pages' full path
for my $file(@files){
  (my $mainfolder, my $filename) = split(/\//, $file); 
  my ($manuscript, $id, $page, $year) = split(/-/, $filename);  
  ($year, my $ext) = split(/\./, $year);
  my $info = ($page. '-' . $year);
  $data{$info} = $file;
}

#Find all moving images, to change the moving image, change the index
if($answer =~ 's'){
  @moving = File::Find::Rule->new
    ->name('*.jpg')
    ->maxdepth('1')
    ->in($dirs[2]);
}
elsif($answer =~ 'm'){
  @moving = File::Find::Rule->new
    ->name('*.jpg')
    ->maxdepth('1')
    ->in($dirs[1]);
}
else{}

#Match and write data to CSV
for my $file(@moving){

  #Name of 1st image
  my($fname, $ext) = split(/\./, $file);
  (my $mainfolder, my $filename) = split(/\//, $fname);
  my ($manuscript, $id, $page, $year) = split(/-/, $filename);
  
  #Keys for both images
  my $key1 = ("$page" . '-' . $dirs[2]);
  my $key2 = ("$page" . '-' . $dirs[1]);
  
  if($answer =~ 's'){
    $key1 = ("$page" . '-' . $dirs[1]);
    $key2 = ("$page" . '-' . $dirs[2]);
  }
  #If page exists in both years...
  if(exists($data{$key1})){
    if (exists($data{$key2})){

      #print 1st image to csv
      print $csv $data{$key1};
      print $csv ", ";

      #Getting width and height difference
      #for first image
      my $orig_name = $data{$key1};
      my $i1 = Image::Magick->new;
      my $x1 = $i1->Read($orig_name);
      my $orig_height = $i1->Get('height');
      my $orig_width = $i1->Get('width');
      
      #splitting the file name and making the tile name
      my($name10, $ext) = split(/\./, $orig_name);
      (my $mainfolder, my $filename) = split(/\//, $name10); 
      my ($manuscript, $id, $page, $year) = split(/-/, $filename); 
      my $tilename = "$mainfolder/$filename/$filename". "_tile1.jpg";
      
      #get the tiles' widith and height
      if (-e $tilename){
        my $i2 = Image::Magick->new;
        my $x2 = $i2->Read($tilename);
        my $tile_height = $i2->Get('height');
        my $tile_width = $i2->Get('width');
      
        #print difference
        $hd1 = $orig_height - $tile_height;
        $wd1 = $orig_width - $tile_width;
        print $csv $wd1, ", ", $hd1, ", ";
      }
      else{
        print $csv $orig_width, ", ", $orig_height, ", ";
      }
      #Info and name for 2nd image
      print $csv $data{$key2};
      print $csv ", ";
      
      #Getting width and height difference
      $orig_name = $data{$key2};
      $i1 = Image::Magick->new;
      $x1 = $i1->Read($orig_name);
      $orig_height = $i1->Get('height');
      $orig_width = $i1->Get('width');

      #reading in and getting width/height for 2nd image tile
      my @parts = split(/\./, $orig_name);
      $filename = $parts[0];
      my @folderFile = split(/\//, $filename);
      $tilename = "$filename/$folderFile[1]". "_tile1.jpg";
      if(-e $tilename){
        my $i2 = Image::Magick->new;
        my $x2 = $i2->Read($tilename);
        my $tile_height = $i2->Get('height');
        my $tile_width = $i2->Get('width');
        
        #printing difference
        $hd2 = $orig_height - $tile_height;
        $wd2 = $orig_width - $tile_width;
        print $csv $wd2, ", ", $hd2, "\n ";
      }
      else{
        print $csv $orig_width, ", ", $orig_height, "\n";
      }
    }
  }
}
