#!/usr/bin/perl
use warnings;
use strict;

sub ltrim { my $s = shift; $s =~ s/^\s+//; return $s};   
while(<>){
  #line should be in format: image # # image # # 
  #The numbers are for use when aligning using tiles

  my @commands = split ", ";

  #Assigns commands to image and trims the space off
  my $fixedimage = $commands[0];
  $fixedimage = ltrim($fixedimage);
  my $floatimage = $commands[3];

  #scale downsizes images by that percent
  my $scale = 0;
  
  #split the images to make the keyfile name
  my ($fixed, $ext1) = split(/\./, $fixedimage);
  my ($float, $ext2) = split(/\./, $floatimage);
  my $fixedkey = $fixed . ".key";
  my $floatkey = $float . ".key";
  
  #check to see if images are defined
  die "fixed $fixedimage doesn't exist\n" if not defined -e $fixedimage;
  die "float $floatimage doesn't exist\n" if not defined -e $floatimage;
  
  #generates the keys and finds matches
  `generatekeys $fixedimage $fixedkey $scale`;
  `generatekeys $floatimage $floatkey $scale`;
  `autopano --integer-coordinates alignment.pto $fixedkey $floatkey`;

  #create landmarks name
  my @fixedparts = split(/-/, $fixedimage);
  my @floatparts = split(/-/, $floatimage);
  
  my @fixedYear = split(/\//, $fixedimage);
  my @floatYearManuscript = split(/\//, $floatparts[0]);
  mkdir("./$floatYearManuscript[0]/landmarks") unless (-d "./$floatYearManuscript[0]/landmarks");
  my $ldmname = "$floatYearManuscript[0]/landmarks/$floatYearManuscript[1]-$floatparts[1]-$floatparts[2]-$floatYearManuscript[0]_Reg$fixedYear[0].ldm";

  #open the landmarks and alignment files
  open(my $landmarks, ">", $ldmname) or die "couldn't open landmarks.ldm\n";
  open(my $alignment, "<", "alignment.pto") or die "couldn't open alignment.pto";
  
  #write the landmarks to file
  for (<$alignment>) {
    print $landmarks "$1 $2 $3 $4\n" if /x(\d+) y(\d+) X(\d+) Y(\d+)/;
  }
}
