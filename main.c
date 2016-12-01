/*
 *TODO
 *indexを増やす ( index生成の関数, 5tappleのどのいちをインデックスにするかの決定 )
 * aaaaa 
 * */
/* header file */
#include "define.h"
int entry_size = 0; //現在のエントリ数を指す
int INDEX_MAX = ENTRY_MAX / WAY_MAX;
int hitflag = 0;
int miss = 0;
/* ファイルから読み取った1行を空白で分割し構造体の各フィールドに代入 */
tapple_t stringSplit( char *tapple_string )
{
	tapple_t tapple;
	char *cp;
	struct in_addr inp;
	
	cp = strtok( tapple_string, " " );
	memcpy( tapple.srcip, cp, 16 );

	cp = strtok( NULL, " " );
	memcpy( tapple.dstip, cp, 16 );

	cp = strtok( NULL, " " );
	memcpy( tapple.protcol, cp, 3 );

	cp = strtok( NULL, " " );
	tapple.srcport = atoi( cp );

	cp = strtok( NULL, " " );
	tapple.dstport = atoi( cp );

	cp = strtok( NULL, " " );
	tapple.reach_time = ( double )atof( cp );

	return tapple;
}

void binaryConvert( tapple_t x, char * bin_tapple )
{
	struct in_addr inp;
	int prot, position, i, tmp, j;
	char eight_byte[8];
	unsigned long tmp_ip;

	if ( inet_aton( x.srcip, &inp ) == 1 )
	{
		position = 0;
		for ( i = 0 ; i < 4 ; i = i + 1 )
		{
			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				if( inp.s_addr % 2 == 1 )
				{
					eight_byte[j] = 49;
				}
				else if ( inp.s_addr % 2 == 0 )
				{
					eight_byte[j] = 48;
				}
				inp.s_addr = inp.s_addr / 2;
			}

			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				bin_tapple[j + 8*i + position] = eight_byte[7 - j];
			}
		}
	}

	if ( inet_aton( x.dstip, &inp ) == 1 )
	{
		position = 32;
		for ( i = 0 ; i < 4 ; i = i + 1 )
		{
			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				if( inp.s_addr % 2 == 1 )
				{
					eight_byte[j] = 49;
				}
				else if ( inp.s_addr % 2 == 0 )
				{
					eight_byte[j] = 48;
				}
				inp.s_addr = inp.s_addr / 2;
			}

			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				bin_tapple[j + 8*i + position] = eight_byte[7 - j];
			}
		}

	}

	if ( strcmp( x.protcol, "TCP" ) == 0 )
	{
		position = 71;
		prot = 6;
		for ( i = 0 ; i < 8 ; i = i + 1 )
		{
			if( prot % 2 == 1 )
			{
				bin_tapple[position] = 49;
			}
			else if ( prot % 2 == 0 )
			{
				bin_tapple[position] = 48;
			}
			position = position - 1;
			prot = prot / 2;
		}
	}
	else if ( strcmp(x.protcol, "UDP") == 0 )
	{
		position = 71;
		prot = 17;
		for ( i = 0 ; i < 8 ; i = i + 1 )
		{
			if( prot % 2 == 1 )
			{
				bin_tapple[position] = 49;
			}
			else if ( prot % 2 == 0 )
			{
				bin_tapple[position] = 48;
			}
			position = position - 1;
			prot = prot / 2;
		}
	}

	position = 87;
	tmp = x.srcport;
	for ( i = 0 ; i < 16 ; i = i + 1 )
	{
		if( tmp % 2 == 1 )
		{
			bin_tapple[position] = 49;
		}
		else if ( tmp % 2 == 0 )
		{
			bin_tapple[position] = 48;
		}
		position = position - 1;
		tmp = tmp / 2;
	}

	position = 103;
	tmp = x.dstport;
	for ( i = 0 ; i < 16 ; i = i + 1 )
	{
		if( tmp % 2 == 1 )
		{
			bin_tapple[position] = 49;
		}
		else if ( tmp % 2 == 0 )
		{
			bin_tapple[position] = 48;
		}
		position = position - 1;
		tmp = tmp / 2;
	}

	bin_tapple[104] = '\0';
//	fprintf( stdout, "%s\n", bin_tapple );
}

int main( int argc, char *argv[] )
{
	char fivetapple[200];
	char bin_tapple[105];
	char * tmp;
	tapple_t tapple;
	int i = 1;
	int index = 0;
	double hit_rate = 0;

	listInit();
	listInitStatic();
	if ( ( inputfile = fopen( argv[1], "r" ) ) == NULL )
	{
		fprintf( stdout, "file open error\n" );
		return 0;
	}

	while( fgets( fivetapple, 200, inputfile ) != NULL )
	{
		tapple = stringSplit( fivetapple );
		binaryConvert( tapple, bin_tapple ); //5tappleを104ビットの2進数に変換する
		index = crcOperation( bin_tapple ); //8ビットのインデックスを作成
		listOperation( tapple, index ); //listに対する操作. シミュレーションのコア部分
		listInsertStatic( tapple, index ); //統計情報を取るためのリストに要素を追加していく
//		fprintf( stdout, "%d\n", index );
//		if ( index == 252 )
//		{
//			if ( hitflag == 1 )
//			{
//				fprintf( stdout, "hit " );
//			}
//			fprintf( stdout, "NO%d - %s %s %s %d %d %f index is %d\n", i, tapple.srcip, tapple.dstip, tapple.protcol, tapple.srcport, tapple.dstport, tapple.reach_time, index );
//			printValue();
//		}
		i = i + 1;
	}
	
//	printValue();
	fclose( inputfile );
	fprintf( stdout, "input file is closed\n" );
//	printValueStaticAll();
	flowStatic(); //入力パケットの統計情報を取る
	hit_rate = (double)hitflag / ( (double)hitflag + (double)miss );
	fprintf( stdout, "hit:%d miss:%d hit rate:%lf\n", hitflag, miss, hit_rate );

	return 0;
}
