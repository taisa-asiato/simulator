#include "define.h"

void printValueCRC( char * crc, char * tmp, int position )
{
	int i = 0;

//	fprintf( stdout, "=====\n" );

	for ( i = 0 ; i < position ; i = i + 1 )
	{
		fprintf( stdout, " " );
	}
	fprintf( stdout, "%s\n", crc );
	fprintf( stdout, "--------------------------------------------------------------------------------------------------------------------------------\n" );
	fprintf( stdout, "%s\n", tmp );
}


/* $BF~NO$7$?J8;zNs(Bbin_tuple$B$+$i(B8$B%S%C%H$N%$%s%G%C%/%9@0?t$r:n@.$9$k(B */
int crcOperation( std::string str_bintuple  )
{
	char CRC8[10] = "111010101"; //$B@8@.B?9`<0(B 
	char tmp_tuple[105];
	int position = 0;
	int xorposition = 0;
	int i = 0, j = 0;
	int index = 0, tmp = 0;

//	fprintf( stdout, "%s\n", bin_tuple );
	strcpy( tmp_tuple, str_bintuple.c_str() );
	while ( position < 96 )
	{
		//$B$3$3$i$X$s$NA`:n$O(B, $B%S%C%H%7%U%H$H$+$r;H$C$F=q$1$P$b$C$H$o$+$j$d$9$/$J$k$N$+!)(B.
		if ( tmp_tuple[position] == '0' )
		{
			position = position + 1;
		}
		else if ( tmp_tuple[position] == '1' )
		{
			xorposition = position;
			for ( i = 0 ;  i < 9 ; i = i + 1 )
			{
				if (
					( tmp_tuple[xorposition + i] == '1' && CRC8[i] == '1' ) ||
					( tmp_tuple[xorposition + i] == '0' && CRC8[i] == '0' )
				   )
				{
					tmp_tuple[xorposition + i] = '0';
				}
				else
				{
					tmp_tuple[xorposition + i] = '1';
				}
			}
		}
//		printValueCRC( CRC8, tmp_tuple, position );
	}

	/* $B%$%s%G%C%/%9$H$J$k$N$O:G8e$+$i(B8bit$BJ,$NCM(B */
	/* 96~103bit$B$,%$%s%G%C%/%9$H$J$k(B */
	for ( i = 7 ; i > -1 ; i = i - 1 )
	{
		if ( tmp_tuple[103 - i] == '0' )
		{
			tmp = 0;
		}
		else if ( tmp_tuple[103 - i] == '1' )
		{
			tmp = 1;
		}

		for ( j = 0 ; j < i ; j = j + 1 )
		{
			tmp = tmp * 2;
		}
		index = index + tmp;
	}

	return index;
}


/* CRC$B$r(BIP$B%"%I%l%9$K$N$_9T$&(B */
int crcOpeforIP( std::string str_bintuple )
{
	char CRC8[10] = "111010101"; //$B@8@.B?9`<0(B 
	char tmp_tuple[105];
	int position = 0;
	int xorposition = 0;
	int i = 0, j = 0;
	int index = 0, tmp = 0;

	strcpy( tmp_tuple, str_bintuple.c_str() );
	while ( position < 24 )
	{
		//$B$3$3$i$X$s$NA`:n$O(B, $B%S%C%H%7%U%H$H$+$r;H$C$F=q$1$P$b$C$H$o$+$j$d$9$/$J$k$N$+!)(B.
		if ( tmp_tuple[position] == '0' )
		{
			position = position + 1;
		}
		else if ( tmp_tuple[position] == '1' )
		{
			xorposition = position;
			for ( i = 0 ;  i < 9 ; i = i + 1 )
			{
				if (
					( tmp_tuple[xorposition + i] == '1' && CRC8[i] == '1' ) ||
					( tmp_tuple[xorposition + i] == '0' && CRC8[i] == '0' )
				   )
				{
					tmp_tuple[xorposition + i] = '0';
				}
				else
				{
					tmp_tuple[xorposition + i] = '1';
				}
			}
		}
//		printValueCRC( CRC8, tmp_tuple, position );
	}


//	fprintf( stdout, "%s\n", tmp_tuple );
//	for ( int i = 0 ; i < 32 ; i++ )
//	{
//		if ( i > 23 )
//			fprintf( stdout, "%c", tmp_tuple[i] );
//	}
//	fprintf( stdout, "\n" );
//
	for ( i = 7 ; i > -1 ; i = i - 1 )
	{
		if ( tmp_tuple[31 - i] == '0' )
		{
			tmp = 0;
		}
		else if ( tmp_tuple[31 - i] == '1' )
		{
			tmp = 1;
		}

		for ( j = 0 ; j < i ; j = j + 1 )
		{
			tmp = tmp * 2;
		}
		index = index + tmp;
	}
//	fprintf( stdout, "%d\n", index );
	return index;
}
