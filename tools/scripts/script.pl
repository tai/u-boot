#!/usr/bin/perl

if (-e $ARGV[0]) {
} else {
    print "file $ARGV[0] not found\n";
    exit 1;
}

$_ = `cat $ARGV[0]`;

my $boilerplate0 = q(#!/bin/sh);
my $boilerplate1 = q(sed -n '1h;1!H;${;g;s/unsigned char pKey[^};]*\};/);
my $boilerplate2 = q(/g;p;}' $1);

if (/unsigned char pKey[^};]*\};/) {
    $pubkey = $&;
    $pubkey =~ s/$/\\/mg;
    print "$boilerplate0\n\n";
    print "$boilerplate1";
    print "$pubkey\n";
    print "$boilerplate2\n";
    exit 0;
} else {
    print "invalid RSA public key in file $ARGV[0]\n";
    exit 2;
}
