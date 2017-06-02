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


/* 入力した文字列bin_tupleから8ビットのインデックス整数を作成する */
int crcOperation( char * bin_tuple )
{
	char CRC8[10] = "111010101\0"; //生成多項式 
	char tmp_tuple[105];
	int position = 0;
	int xorposition = 0;
	int i = 0, j = 0;
	int index = 0, tmp = 0;

//	fprintf( stdout, "%s\n", bin_tuple );
	strcpy( tmp_tuple, bin_tuple );
	while ( position < 96 )
	{
		//ここらへんの操作は, ビットシフトとかを使って書けばもっとわかりやすくなるのか？.
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

	/* インデックスとなるのは最後から8bit分の値 */
	/* 96~103bitがインデックスとなる */
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


/* CRCをIPアドレスにのみ行う */
int crcOpeforIP( char * bin_tuple )
{
	char CRC8[10] = "111010101\0"; //生成多項式 
	char tmp_tuple[105];
	int position = 0;
	int xorposition = 0;
	int i = 0, j = 0;
	int index = 0, tmp = 0;

	strcpy( tmp_tuple, bin_tuple );
	while ( position < 24 )
	{
		//ここらへんの操作は, ビットシフトとかを使って書けばもっとわかりやすくなるのか？.
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
