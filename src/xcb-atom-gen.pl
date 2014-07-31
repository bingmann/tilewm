#!/usr/bin/perl -w
################################################################################
# src/xcb-atom-gen.pl
#
# Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

use strict;
use warnings;

open(F, "xcb.h") or die("Cannot read xcb.h: $!");

my @atomlist = ();
my @cursorlist = ();

while (my $ln = <F>)
{
    if ($ln =~ /static XcbAtom (\S+);/) {
        push(@atomlist, $1);
    }
    if ($ln =~ /static XcbCursor CR_(\S+);/) {
        push(@cursorlist, $1);
    }
}

print <<EOF;
/*******************************************************************************
 * src/xcb-atom.cpp
 *
 * Auto-generated functions to manage named cached atoms and cursors.
 *
 *******************************************************************************
 * Copyright (C) 2014 Timo Bingmann <tb\@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "xcb.h"

EOF

foreach my $atom (@atomlist)
{
    print "XcbConnection::XcbAtom XcbConnection::$atom =\n";
    print "{ \"$atom\", XCB_ATOM_NONE \};\n";
}

my $ewmhnum = scalar grep(/^_NET/, @atomlist);

print <<EOF;

std::vector<xcb_atom_t> XcbConnection::get_ewmh_atomlist()
{
    std::vector<xcb_atom_t> atomlist($ewmhnum);

EOF

my $i = 0;
foreach my $atom (@atomlist)
{
    next unless $atom =~ /^_NET/;
    print "    atomlist[".$i++."] = $atom.atom;\n";
}

print <<EOF;

    return atomlist;
}

struct XcbConnection::XcbAtom* XcbConnection::atomlist[] = {
EOF

print "    &$_,\n" foreach (@atomlist);

print <<EOF;
};

const unsigned int XcbConnection::atomlist_size
    = sizeof(atomlist) / sizeof(*atomlist);

EOF

foreach my $cursor (@cursorlist)
{
    print "XcbConnection::XcbCursor XcbConnection::CR_$cursor =\n";
    print "{ \"$cursor\", XCB_CURSOR_NONE \};\n";
}

print <<EOF;

struct XcbConnection::XcbCursor* XcbConnection::cursorlist[] = {
EOF

print "    &CR_$_,\n" foreach (@cursorlist);

print <<EOF;
};

const unsigned int XcbConnection::cursorlist_size
    = sizeof(cursorlist) / sizeof(*cursorlist);

/******************************************************************************/
EOF
