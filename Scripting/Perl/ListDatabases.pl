#!/usr/bin/perl -w
#
# 2001-11-08  John Marshall  <jmarshall@acm.org>

use strict;
use Getopt::Std;

use EmRPC;
use EmFunctions;

my %opt;
getopt ('', \%opt);

EmRPC::OpenConnection (@ARGV);

my ($cardNo, $i, $lid);

print "card/LocalID\ttype\tcrid\tname\n" unless defined $opt{q};

for $cardNo (0 .. MemNumCards () - 1) {
  for $i (0 .. DmNumDatabases ($cardNo) - 1) {
    my $localID = DmGetDatabase ($cardNo, $i);
    my ($err, %r) = DmDatabaseInfo ($cardNo, $localID);
    printf "%d 0x%08x\t%s\t%s\t%s\n",
           $cardNo, $localID, 
	   pack ("N", $r{type}), pack ("N", $r{creator}), $r{name};
    }
  }

EmRPC::CloseConnection ();
