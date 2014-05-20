#!/usr/bin/perl
use warnings;

die "not enough arguments\n" if scalar @ARGV != 3;

my $land = $ARGV[0];
my $staticImage = $ARGV[1];
my $movingImage = $ARGV[2];
my $crosshair = "~/visDev/registration-toolkit/registration/src/crosshair.png";

@output = `identify $staticImage`;
$output[0] =~ /(\d+)x(\d+)/;
$pointsize = int($1 / 38);
$strokewidth = int($1/1000);
$scale = int($1/45);

system("convert $crosshair -scale $scale" . "x" . "$scale crosshair_intermediate.png");

open(my $landmarksFile, "<", $land);
for my $line(<$landmarksFile>) {
  chomp $line;
  @n = split " ", $line;
  for (0..3) { $n[$_] -= $scale / 2; }
  $number++;
  $convert_static .= " -page +$n[0]+$n[1] crosshair_intermediate.png";
  $convert_static .= " -pointsize $pointsize -fill white -stroke black -strokewidth $strokewidth -draw \"text $n[0],$n[1] '$number'\"";
  $convert_moving .= " -page +$n[2]+$n[3] crosshair_intermediate.png";
  $convert_moving .= " -pointsize $pointsize -fill white -stroke black -strokewidth $strokewidth -draw \"text $n[2],$n[3] '$number'\"";
}

$convert_static .= " -flatten";
$convert_moving .= " -flatten";

system("convert $staticImage $convert_static static_intermediate.jpg");
system("convert $movingImage $convert_moving moving_intermediate.jpg");
system("montage static_intermediate.jpg moving_intermediate.jpg -geometry +0+0 montaged.jpg");
system("rm static_intermediate.jpg moving_intermediate.jpg crosshair_intermediate.png");


