#!/usr/bin/perl


use File::Find;
use File::Basename;

find(
    {
        wanted => \&findfiles,
    },
    'include'
);


find(
    {
        wanted => \&findfiles,
    },
    'src'
);

sub findfiles
{
    $f = $File::Find::name;
    if ($f =~ m/StyleSheet\.h$/)
    {
        return;
    }

    if ($f =~ m/\.cpp$/ || $f =~ m/\.h$/)
    {
        print $f . " \n";
        $q = basename($f);

        open (IN, "< $q") || die "Cant open $q from $f";
        open (OUT, "> $q.bak");
        while(<IN>)
        {
            if (m/getStringWidth/)
            {
                print;
                s/([^\s\{]+)\.getStringWidth\(/SST_STRING_WIDTH_INT\($1, /;
                print;
                #die;
            }
            print OUT;

        }
        close(IN);
        close(OUT);
        system("mv ${q}.bak ${q}");
    }
}
