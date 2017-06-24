/*
 */
/* header file */
#include "define.h"
int entry_size = 0; //$B8=:_$N%(%s%H%j?t$r;X$9(B
int INDEX_MAX = ENTRY_MAX / WAY_MAX;

// $BA4BN$N%R%C%H?t(B
int hitflag = 0;
// $BA4BN$N%_%9?t(B
int miss = 0;
// $B%Q%1%C%H$N%?%$%`%9%?%s%W(B
double arrival_time = 1.0;
// 1$BICJU$j$N%R%C%H?t(B
int hit_per_sec = 0;
// 1$BICJU$j$N%_%9?t(B
int miss_per_sec = 0;
// 1$BICJU$j$N%R%C%HN((B
double hitrate_per_sec[901] = { 0.0 };
// 
double black_time = 0.01; 
int user_number = 0;
unsigned int filerow = 0;
ifstream inputfile; 
node_t * head[ENTRY_MAX / WAY_MAX]; //最初のエントリを指すポインタ
node_t * p[ENTRY_MAX / WAY_MAX]; //エントリの最後を指すポインタ

node_t * head_static[ENTRY_MAX / WAY_MAX]; //統計情報を取るために用いるリストの最初のエントリを指すポインタ配列
node_t * p_static[ENTRY_MAX / WAY_MAX]; //上記のリストのエントリの最後を指すポインタ配列
//本来の情報を登録するリスト
node_t * analyze;
node_t * analyze_end;

//検索用のリスト
node_t * search;
node_t * search_end;
//仮のリストの先頭要素を保持するポインタ配列
//another_node_t * another_tmp_list[ENTRY_MAX / WAY_MAX];
// ブラックリスト, キャッシュエントリに登録しないフローを生成するuserを登録する
black_list_t * blackuser;
black_list_t * blackuser_end;

/* $B%U%!%$%k$+$iFI$_<h$C$?(B1$B9T$r6uGr$GJ,3d$79=B$BN$N3F%U%#!<%k%I$KBeF~(B */
tuple_t stringSplit( char * tuple_string )
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

	return tuple;
}

/* 5$B%?%W%k$NCM$r(B104bit$B$NFs?J?tJQ49(B */
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

	//$B9T?t%+%&%s%HMQ$NJ8;zNs$N:n@.(B
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

	// 1$BICJU$j$N(Bhit$BN($r=PNO$9$k(B
	hitrate_per_sec[(int)arrival_time - 1] = (double)hit_per_sec/( (double)hit_per_sec + (double)miss_per_sec );
	for ( i = 0 ; i < 901 ; i = i + 1 )
	{
		fprintf( stdout, "%f, ", hitrate_per_sec[i] );
	}
	fprintf( stdout, "\n" );
}

int main( int argc, char ** argv )
{
	string fivetuple;
	string bin_tuple;
	tuple_t tuple;
	node_t * tmp_tuple;
	black_list_t * tmp_black_node;
	sent_flow_t * tmp_sent_flow;
	int i = 1;
	int index = 0; //user_number = 0;
	double hit_rate = 0;
	int list_row = 0, tmp;
	//analyze_t analyze[filerow];

	//listInit();
	//listInitStatic();
	//makeBlackList();

	inputfile.open( argv[1] );

	while( !inputfile.eof() )
	{
		getline( inputfile, fivetuple );
//		tuple = stringSplit( fivetuple );
//		binaryConvert( tuple, bin_tuple ); //5tuple$B$r(B104$B%S%C%H$N(B2$B?J?t$KJQ49$9$k(B
//		index = crcOperation( bin_tuple );
		//8$B%S%C%H$N%$%s%G%C%/%9$r:n@.(B
//		index = crcOpeforIP( bin_tuple );
//		tmp_black_node = isUserRegistered( tuple );
//		if ( ( tmp_black_node == NULL ) || ( tmp_black_node->isblackuser == 0 ) )
//		{ 
//			listOperation( tuple, index, argv[2] ); 
//		}
//		fprintf( stdout, "NO%d - %s %s %s %d %d %f index is %d\n", i, tuple.srcip, tuple.dstip, tuple.protcol, tuple.srcport, tuple.dstport, tuple.reach_time, index );
//		fprintf( stdout, "%s, %s, %s, %d, %d, %f, %d\n", tuple.srcip, tuple.dstip, tuple.protcol, tuple.srcport, tuple.dstport, tuple.reach_time, index );
//		blackListOperation( tuple );
//		printRegisteredBlackList();
//		tmp = listInsertStatic( analyze_end, tuple, index ); //$BE}7W>pJs$r<h$k$?$a$N%j%9%H$KMWAG$rDI2C$7$F$$$/(B
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
//	fclose( inputfile );
//	fprintf( stdout, "input file is closed\n" );
//	flowStaticMain(); //$BF~NO%Q%1%C%H$NE}7W>pJs$r<h$k(B
//	printValueStaticAll();
//	hit_rate = (double)hitflag / ( (double)hitflag + (double)miss );
//	fprintf( stdout, "hit:%d miss:%d hit rate:%lf\n", hitflag, miss, hit_rate );

//	printHitrate();

	return 0;
}
