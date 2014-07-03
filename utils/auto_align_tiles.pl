#!/usr/bin/perl
use warnings;
use strict;
use String::Util 'trim';
use File::Spec;

sub ltrim { my $s = shift; $s =~ s/^\s+//;       return $s };

my $floatname;
my $fixedyear;

#image wd hd image wd hd

while (<>){
  chomp;
  my @commands = split ", ";

  my $wholefixedimage = $commands[0];
  $wholefixedimage = ltrim($wholefixedimage);
  my($wholefixed, $ext)= split(/\./, $wholefixedimage);
  ($fixedyear, my $fixedname) = split(/\//, $wholefixed);

  my $fixed_wd = $commands[1];
  my $fixed_hd = $commands[2];

  my $wholefloatimage = $commands[3];
  $wholefloatimage = ltrim($wholefloatimage);

  my ($wholefloat, $ext2) = split(/\./, $wholefloatimage);
  (my $floatyear, $floatname) = split(/\//, $wholefloat);

  my $float_wd = $commands[4];
  my $float_hd = $commands[5];

  chdir("./$floatyear");
  mkdir("./landmarks") unless (-d "./landmarks");
  chdir("./landmarks");

  open (my $tilelandmarks, ">", "tilelandmarks.ldm") or die "coudldn't open tilelandmarks.ldm";
  my $scale = 700;

  for my $tile(1..5){
    chdir('..');
    chdir('..');
    my $floattemppath = "./$wholefloat/$floatname" . "_tile$tile" . ".jpg";
    my $fixedtemppath = "./$wholefixed/$fixedname" . "_tile$tile".".jpg";

    my $floatimage = File::Spec->rel2abs($floattemppath);
    my $fixedimage = File::Spec->rel2abs($fixedtemppath);
;
    my $fixedkey = $fixedimage;
    $fixedkey =~ s/[.]jpg$/.key/i;
    my $floatkey = $floatimage;
    $floatkey =~  s/[.]jpg$/.key/i;

    die "$fixedimage doesn't exist\n" if not defined -e $fixedimage;
    die "$floatimage doesn't exist\n" if not defined -e $floatimage;

   chdir("./$floatyear/landmarks");

    print "Starting Keys\n";
    `generatekeys $fixedimage $fixedkey $scale`;
    print "Finished 1 Key, 1 more to go\n";
    `generatekeys $floatimage $floatkey $scale`;
    print "Finished 2 keys, starting autopano\n";
    `autopano --maxmatches '3'  --integer-coordinates alignment.pto $fixedkey $floatkey`;
    print "finished autopano\n";

    open(my $alignment, "<", "alignment.pto") or die "couldn't open alignment.pto";

    for(<$alignment>){
      print $tilelandmarks "$1 $2 $3 $4 tile$tile\n" if  /x(\d+) y(\d+) X(\d+) Y(\d+)/;
    }
  }
  close($tilelandmarks);
  open($tilelandmarks, "<", "tilelandmarks.ldm") or die "couldn't open tilelandmarks.ldm";
  my $ldmfilename = "$floatname" . "_Reg$fixedyear" . ".ldm";
  open(my $landmarks, ">", "$ldmfilename") or die "couldn't open new landmarks file";
  my ($t1 ,$t2, $t3, $t4, $t5) = (0,0,0,0,0);
  while(<$tilelandmarks>){
      my @coords = split " ";
      if ($coords[4] =~ /tile1/){
        if ($t1 <= 1){
          print $landmarks "$coords[0] $coords[1] $coords[2] $coords[3]\n";
          $t1 += 1;
        }
      }
      elsif($coords[4] =~ /tile2/){
        if ($t2 <= 1){
          my $x = $coords[0];
          $x += $fixed_wd;
          my $X = $coords[2];
          $X += $float_wd;
          print $landmarks "$x $coords[1] $X $coords[3]\n";
          $t2+=1;
        }
      }
      elsif($coords[4] =~ /tile3/){
        if($t3 <= 1){
          my $x = $coords[0]; 
          $x += $fixed_wd;
          my $y = $coords[1];
          $y += $fixed_hd;
          my $X = $coords[2]; 
          $X += $float_wd;
          my $Y = $coords[3];
          $Y += $float_hd;
          print $landmarks "$x $y $X $Y\n";
          $t3+=1;
        }
      }
      elsif($coords[4] =~ /tile4/){
        if($t4 <= 1){
          my $y = $coords[1]; 
          $y += $fixed_hd;
          my $Y = $coords[3];  
          $Y += $float_hd; 
          print $landmarks "$coords[0] $y $coords[2] $Y\n";
          $t4+=1;
        }  
      }
      elsif($coords[4] =~ /tile5/){
        if($t5 <= 1){
          my $x = $coords[0];   
          $x += int($fixed_wd/2);   
          my $y = $coords[1];
          $y += int($fixed_hd/2);
          my $X = $coords[2];  
          $X += int($float_wd/2); 
          my $Y = $coords[3];
          $Y += int($float_hd/2);
          print $landmarks "$x $y $X $Y\n";
          $t5+=1;
        }  
      }
      else{}
    }
  unlink($tilelandmarks);
  chdir('..');
  chdir('..');
}
