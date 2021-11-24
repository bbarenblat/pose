#!/usr/bin/perl -w
# Lists all databases open in the current thread, topmost first.  Obscure
# fields are open count (#) and the mode with which the db was opened (see
# &openflags below).
#
# 2001-10-25  John Marshall  <jmarshall@acm.org>

use strict;

use EmRPC;
use EmFunctions;

EmRPC::OpenConnection (@ARGV);

sub openflags {
  my ($f) = @_;
  local $_ = "";
  $_ .= "R" if $f & dmModeReadOnly;
  $_ .= "W" if $f & dmModeWrite;
  $_ .= "L" if $f & dmModeLeaveOpen;
  $_ .= "E" if $f & dmModeExclusive;
  $_ .= "S" if $f & dmModeShowSecret;
  return $_;
  }

print "DmOpenRef   card/LocalID  #  mode  r/type/crid  name\n";

my $db = 0;
while (($db = DmNextOpenDatabase ($db)) != 0) {
  printf "0x%08x  ", $db;

  my ($err, %r) = DmOpenDatabaseInfo ($db);

  if ($err == 0) {
    printf "%d 0x%08x%3d  %-5s %s ",
	   $r{cardNo}, $r{dbID}, $r{openCount}, openflags($r{mode}),
	   $r{resDB}? "R" : "D";

    my ($err, %r) = DmDatabaseInfo ($r{cardNo}, $r{dbID});

    if ($err == 0) {
      $_ = pack "NcN", $r{type}, ord " ", $r{creator};
      s/[[:^print:]]/./g;
      print "$_  $r{name}";
      }
    else { printf "[DmDatabaseInfo failed: 0x%x]", $err; }
    }
  else { printf "[DmOpenDatabaseInfo failed: 0x%x]", $err; }

  print "\n";
  }

EmRPC::CloseConnection ();
