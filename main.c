/*
 */
/* header file */
#include "define.h"
int entry_size = 0; //現在のエントリ数を指す
int WAY_MAX = ENTRY_MAX / INDEX_MAX;

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
double userlist_init_time; 
int user_number = 0;
unsigned int filerow = 0;
/*----------チューニング用パラメータ----------*/
// ブラックリストに登録できる最大のuser数
int 	USER_MAX;
// ブラックリストに登録された各userの生成したフローの最大登録数		
int	FLOW_MAX;
// ブラックリストに登録されているuserの生成したflowのパケット数の閾値	
int	THRESHOLD;
// UserListの初期化間隔						
double	USERLIST_INIT_INTERVAL;
/*==========================================*/

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

	/* ---------- チューニング用パラメタ ----------------------------------*/
	// ブラックリストに登録できる最大のuser数				
	USER_MAX = atoi( argv[4] );
	// ブラックリストに登録された各userの生成したフローの最大登録数		
	FLOW_MAX = atoi( argv[5] );
	// ブラックリストに登録されているuserの生成したflowのパケット数の閾値	
	THRESHOLD = atoi( argv[6] );
	// UserListの初期化間隔						
	USERLIST_INIT_INTERVAL = atof( argv[7] );
	/*---------------------------------------------------------------------*/
	userlist_init_time = USERLIST_INIT_INTERVAL;


	char fivetuple[200];
	char bin_tuple[105];
	tuple_t tuple;
	node_t * tmp_tuple;
	user_list_t * tmp_user_node;
	sent_flow_t * tmp_sent_flow;
	long i = 0;
	int index = 0; //user_number = 0;
	double hit_rate = 0, hit_rate_all = 0;
	int list_row = 0, tmp;
	int skip = 0, flag = 0, j = 0;

	//analyze_t analyze[filerow];
	if ( 1 )
	{
		fprintf( stdout, "input file is %s\nPolicy:%s\nblacklist:%s\ndebug:%s\n", argv[1], argv[2], argv[3], argv[8] );
		fprintf( stdout, "user_max %d\nflow_max %d\nThreshold %d\nINTERVAL %f\n", 
				USER_MAX, FLOW_MAX, THRESHOLD, USERLIST_INIT_INTERVAL );
	}

	listInit();
	listInitStatic();
	makeUserList();

	getInputFileRow( argv[1] );
	fprintf( stdout, "getInputFileRow %d\n", filerow );
	ary_t * flowary;
	flowary = ( ary_t * )malloc( filerow * sizeof( ary_t ) );

	if ( ( inputfile = fopen( argv[1], "r" ) ) == NULL )
	{
		fprintf( stdout, "!ERROR! : file open error\n" );
		return 0;
	}

//	if ( strcpy( argv[8], "remove" ) == 0 )
//	{
		//flowListInit();
		while ( fgets( fivetuple, 250, inputfile ) != NULL )
		{
			flag++;
			if ( flag % 10000 == 0 )
			{
				fprintf( stdout, "%d\n", flag );
			}
			tuple = stringSplit( fivetuple );
			i = flowAryUpdate( tuple, flowary, j );
			if ( i == -1 )
			{
				fprintf( stdout, "!ERROR! : Update Failed\n" );
				break;
			}
			j = j + i;
		}
		fprintf( stdout, "flow num:%d\n", j );
		fclose( inputfile );
//	}

//	printOnly1pFlow();
	if ( ( inputfile = fopen( argv[1], "r" ) ) == NULL )
	{
		fprintf( stdout, "!ERROR! : file open error\n" );
		return 0;
	}

	fprintf( stdout, "%s\n", argv[8] );
	while( fgets( fivetuple, 250, inputfile ) != NULL )
	{
		tuple = stringSplit( fivetuple );
		binaryConvert( tuple, bin_tuple ); //5tupleを104ビットの2進数に変換する
		index = crcOperation( bin_tuple );
		
//		if ( argc == 8 )
//		{
//			listOperation( tuple, index, argv[2], argv[3], argv[8] );
//		}
//		else
//		{
//			if ( strcmp( argv[8], "remove" ) == 0 )
//			{		
//				flag = flowListSearch( tuple, index );
//				if ( flag >= 0 )
//				{
//					if ( flag == 1 )
//					{
//						skip++;
//					}
//					else 
//					{
//						listOperation( tuple, index, argv[2], argv[3], argv[8] );
//					}
//				}
//				else 
//				{
//					fprintf( stdout, "!ERROR! : List Create Failed\n" );
//					break;
//				}
//			}
//		}
		//8ビットのインデックスを作成
//		index = crcOpeforIP( bin_tuple );
//
//
//		fprintf( stdout, "NO%d - %s %s %s %d %d %f index is %d\n", i, tuple.srcip, tuple.dstip, tuple.protcol, tuple.srcport, tuple.dstport, tuple.reach_time, index );
//		listOperation( tuple, index, argv[2], argv[3], argv[8] );
		//printUserList();
//		printValueIndex( index );
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
	if ( 1 )//strcmp( argv[8], "debug") != 0 )
	{
		hit_rate = (double)hitflag / ( (double)hitflag + (double)miss );
		hit_rate_all = (double)hitflag / (double)i;
		fprintf( stdout, "hit:%d miss:%d hit rate:%lf\n", hitflag, miss, hit_rate );
		fprintf( stdout, "all packet:%ld hit rate per all packet:%lf\n", i, hit_rate_all );
		printHitrate();
	}

	return 0;
}
