/*
 */
/* header file */
#include "define.h"
int entry_size = 0; //現在のエントリ数を指す
int INDEX_MAX = ENTRY_MAX / WAY_MAX;

// 全体のヒット数
int hitflag = 0;
// 全体のミス数
int miss = 0;
// パケットのタイムスタンプ
double time = 1.0;
// 1秒辺りのヒット数
int hit_per_sec = 0;
// 1秒辺りのミス数
int miss_per_sec = 0;
// 1秒辺りのヒット率
double hitrate_per_sec[901] = { 0.0 };
// 
double blacklist_init_time = BLACKLIST_INIT_INTERVAL; 
int user_number = 0;
unsigned int filerow = 0;

/* ファイルから読み取った1行を空白で分割し構造体の各フィールドに代入 */
tuple_t stringSplit( char *tuple_string )
{
	tuple_t tuple;
	char *cp;
	struct in_addr inp;
	
	cp = strtok( tuple_string, " " );
	memcpy( tuple.srcip, cp, 16 );

	cp = strtok( NULL, " " );
	memcpy( tuple.dstip, cp, 16 );

	cp = strtok( NULL, " " );
	memcpy( tuple.protcol, cp, 3 );

	cp = strtok( NULL, " " );
	tuple.srcport = atoi( cp );

	cp = strtok( NULL, " " );
	tuple.dstport = atoi( cp );

	cp = strtok( NULL, " " );
	tuple.reach_time = ( double )atof( cp );

//	fprintf( stdout, "stringSplit finished\n" );

	return tuple;
}

/* 5タプルの値を104bitの二進数変換 */
void binaryConvert( tuple_t x, char * bin_tuple )
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
				bin_tuple[j + 8*i + position] = eight_byte[7 - j];
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
				bin_tuple[j + 8*i + position] = eight_byte[7 - j];
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
				bin_tuple[position] = 49;
			}
			else if ( prot % 2 == 0 )
			{
				bin_tuple[position] = 48;
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
				bin_tuple[position] = 49;
			}
			else if ( prot % 2 == 0 )
			{
				bin_tuple[position] = 48;
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
			bin_tuple[position] = 49;
		}
		else if ( tmp % 2 == 0 )
		{
			bin_tuple[position] = 48;
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
			bin_tuple[position] = 49;
		}
		else if ( tmp % 2 == 0 )
		{
			bin_tuple[position] = 48;
		}
		position = position - 1;
		tmp = tmp / 2;
	}

	bin_tuple[104] = '\0';
//	fprintf( stdout, "%s\n", bin_tuple );
}

void getInputFileRow( char * filename )
{
	FILE * fp;
	char cmd[50];
	char line[100];
	char * tmp;
	unsigned int num;

	//行数カウント用の文字列の作成
	sprintf( cmd, "wc -l %s", filename );
	if( ( fp = popen( cmd, "r" ) ) == NULL )
	{
		fprintf( stdout, "popen error\n" );
	}
	
	if ( fgets( line, 100, fp ) == NULL )
	{
		fprintf( stdout, "fgets error\n" );
	}

	tmp = strtok( line, " " );
	filerow = atoi( tmp );
	pclose( fp );
}

void printHitrate()
{
	int i;

	// 1秒辺りのhit率を出力する
	hitrate_per_sec[(int)time - 1] = (double)hit_per_sec/( (double)hit_per_sec + (double)miss_per_sec );
	for ( i = 0 ; i < 901 ; i = i + 1 )
	{
		fprintf( stdout, "%f, ", hitrate_per_sec[i] );
	}
	fprintf( stdout, "\n" );
}

int main( int argc, char *argv[] )
{
	char fivetuple[200];
	char bin_tuple[105];
	tuple_t tuple;
	node_t * tmp_tuple;
	user_list_t * tmp_user_node;
	sent_flow_t * tmp_sent_flow;
	int i = 1;
	int index = 0; //user_number = 0;
	double hit_rate = 0;
	int list_row = 0, tmp;
	//analyze_t analyze[filerow];
	fprintf( stdout, "input file is %s\nPolicy:%s\n blacklist:%s\n", argv[1], argv[2], argv[3] );
	fprintf( stdout, "user_max %d\nflow_max %d\nThreshold %d\nINTERVAL %f\n", 
		BLACKUSER_MAX, FLOW_MAX, THRESHOLD, BLACKLIST_INIT_INTERVAL );

	listInit();
	listInitStatic();
	makeUserList();

	if ( ( inputfile = fopen( argv[1], "r" ) ) == NULL )
	{
		fprintf( stdout, "file open error\n" );
		return 0;
	}

	while( fgets( fivetuple, 250, inputfile ) != NULL )
	{
		tuple = stringSplit( fivetuple );
		binaryConvert( tuple, bin_tuple ); //5tupleを104ビットの2進数に変換する
		index = crcOperation( bin_tuple );
		//8ビットのインデックスを作成
//		index = crcOpeforIP( bin_tuple );
//		tmp_black_node = isUserRegistered( tuple );
//		if ( ( tmp_black_node == NULL ) || ( tmp_black_node->isblackuser == 0 ) )
//		{ 
//
//
//		fprintf( stdout, "NO%d - %s %s %s %d %d %f index is %d\n", i, tuple.srcip, tuple.dstip, tuple.protcol, tuple.srcport, tuple.dstport, tuple.reach_time, index );
		listOperation( tuple, index, argv[2], argv[3] ); 
//		printBlackList();
//		printBlackUser();
//		}
//		blackListOperation( tuple );
//		printRegisteredBlackList();
//		tmp = listInsertStatic( analyze_end, tuple, index ); //統計情報を取るためのリストに要素を追加していく
//		listSearchStatic( tuple, index );
//		list_row = list_row + tmp;
//		flowStatic();
//		fprintf( stdout, "%d\n", index );
//		if ( index == 252 )
//		{
//			if ( hitflag == 1 )
//			{
//				fprintf( stdout, "hit " );
//			}
			//			printValue();
//		}
//		fprintf( stdout, "user num :%d\n", user_number );
//		printRegisteredBlackList();
		i = i + 1;
	}
	
//	printValue();
	fclose( inputfile );
//	fprintf( stdout, "input file is closed\n" );
//	flowStaticMain(); //入力パケットの統計情報を取る
//	printValueStaticAll();
	hit_rate = (double)hitflag / ( (double)hitflag + (double)miss );
	fprintf( stdout, "hit:%d miss:%d hit rate:%lf\n", hitflag, miss, hit_rate );

	printHitrate();

	return 0;
}
