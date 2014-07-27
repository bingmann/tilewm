#!/usr/bin/perl -w
################################################################################
# src/xcb-ostream-gen.pl
#
# Script to auto generate ostream operator << implementation for xcb structures
#
# The script uses gccxml to "compile" xcb-ostream-stub.cpp into an XML file,
# which contains information about the fields in all XCB structs. TODO: A
# better solution may be to read the Xorg XML protocol definition.
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

use IO::File;
use XML::Simple qw(:strict);
use Data::Dumper;

my $fh = IO::File->new("gccxml xcb-ostream-stub.cpp -fxml=/dev/stdout |")
    or die("Cannot run gccxml: $!");

my $ref = XMLin($fh, ForceArray => 1, KeyAttr => [])
    or die("Could not parse XML output of gccxml: $!");

if ($ARGV[0] and $ARGV[0] eq "dump") {
    print Dumper($ref);
    exit;
}

my $trace = 0;

sub find_typeid {
    my ($typeid) = @_;

    foreach my $ftype (@{$$ref{Typedef}})
    {
        $$ftype{id} or next;
        $$ftype{id} eq $typeid or next;

        print STDERR "type: $$ftype{id} - $$ftype{name}\n" if $trace;
        return $$ftype{name};
    }
    return "undef";
}

sub find_field {
    my ($fieldid) = @_;

    foreach my $key (qw(Field Constructor Destructor OperatorMethod))
    {
        foreach my $field (@{$$ref{$key}})
        {
            $$field{id} or next;
            $$field{id} eq $fieldid or next;

            print STDERR "$$field{id} - $$field{name}\n" if $trace;
            print STDERR Dumper($field) if $trace;

            return $field;
        }
    }

    return undef;
}

sub process {
    my ($name,$shname,$trace) = @_;

    my $out = "";

    foreach my $str (@{$$ref{Struct}})
    {
        $$str{name} or next;
        $$str{name} eq $name or next;

        print STDERR $$str{name}."\n" if $trace;
        print STDERR Dumper($str) if $trace;

        my $pname = $$str{name};
        $pname =~ s/_t$//;

        $out .= "std::ostream&\n";
        $out .= "operator << (std::ostream& os, const $$str{name}& $shname)\n";
        $out .= "{\n";
        $out .= "    os << \"[$pname:\"\n";

        foreach my $id (split(/ /,$$str{members}))
        {
            print "id: $id\n" if $trace;

            my $field = find_field($id) or die("Could not find field $id");

            next if !$$field{type};
            next if $$field{name} =~ /^pad[0-9]$/;

            my $type = find_typeid($$field{type});

            $out .= "       << \" $$field{name}=\" << ";

            if ($type eq "int8_t")
            {
                $out .= "int32_t($shname.$$field{name})\n";
            }
            elsif ($type eq "uint8_t" or $type eq "xcb_keycode_t")
            {
                $out .= "uint32_t($shname.$$field{name})\n";
            }
            elsif ($type eq "xcb_atom_t")
            {
                $out .= "AtomFormatted($shname.$$field{name})\n";
            }
            else
            {
                $out .= "$shname.$$field{name}\n";
            }
        }

        $out .= "       << \"]\";\n";
        $out .= "    return os;\n";
        $out .= "}\n\n";
    }

    die("Could not find $name structure.") unless $out;

    print $out;
}

print <<EOF;
/*******************************************************************************
 * src/xcb-ostream.cpp
 *
 * Auto-generated ostream operators for many XCB structures.
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
#include <ostream>
#include <xcb/xinerama.h>
#include <xcb/randr.h>

EOF

foreach my $header (glob("*.{h,cpp}"))
{
    open(F, $header) or die("Could not open $header: $!");
    my $source = join("", <F>);
    close(F);

    $source =~ s!^.*BEGIN Auto(.+?)END Auto.*$!$1!s or next;

    while ($source =~ m!operator << \(\s*std::ostream& os,\s*const (\S+?)& (\S+?)\)!g)
    {
        process($1, $2);
    }
}

print <<EOF;
/******************************************************************************/
EOF
