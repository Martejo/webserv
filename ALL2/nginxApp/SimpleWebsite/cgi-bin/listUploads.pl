#!/usr/bin/perl
use strict;
use warnings;
use CGI qw(:standard);
use File::Basename;

# Dossier contenant les fichiers multimédias
my $dir = 'uploads/';
opendir(my $dh, $dir) or die "Impossible d'ouvrir le dossier '$dir': $!";

# Récupérer tous les fichiers du dossier
my @files = readdir($dh);
closedir($dh);

# Début du document HTML
print header('text/html; charset=UTF-8');
print start_html(-title => 'Galerie Multimédia', -charset => 'UTF-8');
print "<h1 style='text-align: center;'>Galerie Multimédia</h1>";
print "<div class='media-container' style='display: flex; flex-wrap: wrap; justify-content: center;'>";

# Traiter chaque fichier
foreach my $file (@files) {
    next if ($file =~ /^\.\.?$/); # Ignorer . et ..

    my $ext = lc((fileparse($file, qr/\.[^.]*/))[2]);

    print "<div class='media-item' style='margin: 10px; text-align: center;'>";

    if ($ext =~ /\.(jpg|jpeg|png|gif)$/) {
        print "<img src='$dir$file' style='max-width: 200px; max-height: 200px;' alt='$file'>";
    } elsif ($ext =~ /\.(mp4|webm)$/) {
        print "<video controls style='max-width: 200px;'>
                    <source src='$dir$file' type='video/$ext'>
                    Votre navigateur ne prend pas en charge la lecture de vidéos.
                </video>";
    } elsif ($ext =~ /\.(mp3|wav)$/) {
        print "<audio controls style='max-width: 200px;'>
                    <source src='$dir$file' type='audio/$ext'>
                    Votre navigateur ne prend pas en charge l'audio.
                </audio>";
    }

    print "<p>$file</p>";
    print "</div>";
}

print "</div>";
print end_html;
