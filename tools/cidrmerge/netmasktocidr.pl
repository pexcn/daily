#!/usr/bin/perl -w

$separator=' ';

while (<STDIN>)
{
	chomp;
	($ip,$netmask)=split($separator);

	@res=split('\.',$netmask);
	
	$i=0;
	$mask=0;
	while ($i<4)
	{
		$mask+=$res[$i]<<((3-$i)*8);
		$i++;
	}
        $i=0;
        while($mask>0)
	{
		$mask<<=1;
		$i++;
	}
        $mask=$i;

	print "$ip/$mask\n";

}
