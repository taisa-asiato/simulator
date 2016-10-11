#include "define.h"

/* 入力した文字列bin_tappleから8ビットのインデックス整数を作成する */
int crcOperation( char * bin_tapple )
{
	char CRC8[9] = "110001101"; //生成多項式 
	char tmp_tapple[104];
	int position = 0;
	int xorposition = 0;
	int i = 0, j = 0;
	int index = 0, tmp = 0;
	
	strcpy( tmp_tapple, bin_tapple );
	while ( position < 96 )
	{
		//ここらへんの操作は, ビットシフトとかを使って書けばもっとわかりやすくなる.
		//ので, 少し修正スべき
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

	/* インデックスとなるのは最後から8bit分の値 */
	/* 96~103bitがインデックスとなる */
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
