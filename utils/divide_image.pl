#!/usr/bin/perl
use warnings;
use strict;
use Image::Magick;
use File::Basename;
use File::Find::Rule;

#This program divides an image into 5 tiles to make sure autopano
#generates landmarks in each corner and in the middle of an image.
#It ensures a more even distributation and increases the accuracy 
#of registration.

#finding .jpg files to split
my $root = '.';

my @files = File::Find::Rule->new
  ->name('*.jpg')
  ->in($root);

#reading in images t0 imagemagic
for my $file(@files){
  my $i1 = Image::Magick->new;
  my $i2 = Image::Magick->new;
  my $i3 = Image::Magick->new;
  my $i4 = Image::Magick->new;
  my $i5 = Image::Magick->new;

  my $x1 = $i1->Read($file);
  my $x2 = $i2->Read($file);
  my $x3 = $i3->Read($file);
  my $x4 = $i4->Read($file);
  my $x5 = $i5->Read($file);

  #split filename
  my ($filename, $dir, $ext) = fileparse($file, qr/\.[^.]*/);
  my($mscript, $id, $page, $year) = split(/-/, $filename);
  
  #make a folder for each image
  chdir("./$year");
  mkdir "./$filename";
  chdir("./$filename");

  #Crop the image 5 times, into 5 tiles, setting the colorspace to 
  #work with autopano and writing the tile
  $i1->Crop(geometry=>'33%x33%+0+0'); 
  $i1->Quantize(colorspace=>'RGB');
  $i1->Set(type=>'TrueColor');
  $i1->Write("$filename" . "_tile1.jpg");

  $i2->Crop(gravity=>'NorthEast', geometry=>'33%x33%+0+0');
  $i2->Quantize(colorspace=>'RGB');
  $i2->Set(type=>'TrueColor');
  $i2->Write("$filename" . "_tile2.jpg");

  $i3->Crop(gravity=>'SouthEast', geometry=>'33%x33%+0+0');
  $i3->Quantize(colorspace=>'RGB');
  $i3->Set(type=>'TrueColor');
  $i3->Write("$filename" . "_tile3.jpg");

  $i4->Crop(gravity=>'SouthWest', geometry=>'33%x33%+0+0');
  $i4->Quantize(colorspace=>'RGB');
  $i4->Set(type=>'TrueColor');
  $i4->Write("$filename" . "_tile4.jpg");

  $i5->Crop(gravity=>'Center', geometry=>'33%x33%+0+0');
  $i5->Quantize(colorspace=>'RGB');
  $i5->Set(type=>'TrueColor');
  $i5->Write("$filename" . "_tile5.jpg");
  
  chdir("..");
  chdir("..");
}



