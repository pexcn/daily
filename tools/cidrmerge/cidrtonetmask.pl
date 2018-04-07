#!/usr/bin/perl -w

$separator='/';
$out_separator=' ';

while (<STDIN>)
{
	chomp;
	($ip,$netmask)=split($separator);
	if (!defined($netmask))
	{
		#print STDERR "Invalid line $_\n";
		#next;

		$netmask=32;
	}

	$mask=0xffffffff<<(32-$netmask);	

	$buf="";
	$i=0;
	while ($i<4)
	{
		$buf.=$mask>>((3-$i)*8)&0xff;
		$buf.=".";
		$i++;
	}
	chop $buf;

	print "$ip$out_separator$buf\n";

}
