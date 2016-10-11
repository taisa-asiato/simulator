#include "define.h"

/* $BF~NO$7$?J8;zNs(Bbin_tapple$B$+$i(B8$B%S%C%H$N%$%s%G%C%/%9@0?t$r:n@.$9$k(B */
int crcOperation( char * bin_tapple )
{
	char CRC8[9] = "110001101"; //$B@8@.B?9`<0(B 
	char tmp_tapple[104];
	int position = 0;
	int xorposition = 0;
	int i = 0, j = 0;
	int index = 0, tmp = 0;
	
	strcpy( tmp_tapple, bin_tapple );
	while ( position < 96 )
	{
		//$B$3$3$i$X$s$NA`:n$O(B, $B%S%C%H%7%U%H$H$+$r;H$C$F=q$1$P$b$C$H$o$+$j$d$9$/$J$k(B.
		//$B$N$G(B, $B>/$7=$@5%9$Y$-(B
		if ( tmp_tapple[position] == '0' )
		{
			position = position + 1;
		}
		else if ( tmp_tapple[position] == '1' )
		{
			xorposition = position;
			for ( i = 0 ;  i < strlen( CRC8 ) ; i = i + 1 )
			{
				if (
					( tmp_tapple[xorposition + i] == '1' && CRC8[i] == '1' ) ||
					( tmp_tapple[xorposition + i] == '0' && CRC8[i] == '0' )
				   )
				{
					tmp_tapple[xorposition + i] = '0';
				}
				else
				{
					tmp_tapple[xorposition + i] = '1';
				}
			}
		}
	}

	/* $B%$%s%G%C%/%9$H$J$k$N$O:G8e$+$i(B8bit$BJ,$NCM(B */
	/* 96~103bit$B$,%$%s%G%C%/%9$H$J$k(B */
	for ( i = 7 ; i > -1 ; i = i - 1 )
	{
		if ( tmp_tapple[103 - i] == '0' )
		{
			tmp = 0;
		}
		else if ( tmp_tapple[103 - i] == '1' )
		{
			tmp = 1;
		}

		for ( j = 0 ; j < i ; j = j + 1 )
		{
			tmp = tmp * 2;
		}
		index = index + tmp;
	}

	fprintf( stdout, "%d\n", index );

	return index;
}
