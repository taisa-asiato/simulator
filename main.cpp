/*
 */
/* header file */
#include "define.h"

using namespace std;

int entry_size = 0; //$B8=:_$N%(%s%H%j?t$r;X$9(B
int WAY_MAX = ENTRY_MAX / INDEX_MAX;

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
// 現在時刻を保持する
double userlist_init_time = 0.01; 
// UserListに登録されているuser数を保持する
int user_number = 0;

unsigned int filerow = 0;
// キャッシュに登録しなかったフロー(パケット)数(合計)
int skipflow = 0;
// 上記中, 1pフローであった数(合計)
int onepflow = 0;
// 単位時間あたりのキャッシュに登録しなかった1pフロー数
double hit_1p = 0;
// 単位時間あたりのキャッシュに登録しなかったパケット数
double skip_1p = 0;
node_t * head[INDEX_MAX]; //最初のエントリを指すポインタ
node_t * p[INDEX_MAX]; //エントリの最後を指すポインタ

node_t * head_static[INDEX_MAX]; //統計情報を取るために用いるリストの最初のエントリを指すポインタ配列
node_t * p_static[INDEX_MAX]; //上記のリストのエントリの最後を指すポインタ配列
//本来の情報を登録するリスト
node_t * analyze;
node_t * analyze_end;

//検索用のリスト
node_t * static_search;
node_t * search_end;

//仮のリストの先頭要素を保持するポインタ配列
//another_node_t * another_tmp_list[ENTRY_MAX / WAY_MAX];
// ブラックリスト, キャッシュエントリに登録しないフローを生成するuserを登録する
user_list_t * userlist;
user_list_t * userlist_end;

// フローとそのパケット数を保持するハッシュテーブル
std::unordered_map< std::string, int > ump_tuple;

// 時間によって1pフローの識別率を保持する動的配列
vector< double > identify_rate;

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

tuple_t return_tuple( vector<string> v )
{
	tuple_t tuple;
	tuple.srcip = v[0];
	tuple.dstip = v[1];
	tuple.protcol = v[2];
	tuple.srcport = atoi( v[3].c_str() );
	tuple.dstport = atoi( v[4].c_str() );
	tuple.reach_time = atof( v[5].c_str() );
	return tuple;
}

/* 5$B%?%W%k$NCM$r(B104bit$B$NFs?J?tJQ49(B */
void binaryConvert( tuple_t x, char * bin_tuple )
{
	struct in_addr inp;
	int prot, position, i, tmp, j;
	char eight_byte[8];
	unsigned long tmp_ip;

	if ( inet_aton( x.srcip.c_str(), &inp ) == 1 )
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

	if ( inet_aton( x.dstip.c_str(), &inp ) == 1 )
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

	if ( x.protcol == "TCP" )
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
	else if ( x.protcol ==  "UDP" )
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

vector<string> split( const string str, char sep )
{
	vector<string> v;
	stringstream ss( str );
	string buffer;
	while( getline( ss, buffer, sep ) )
	{
		v.push_back( buffer );
	}

	return v;
}

tuple_t substituteTuple( vector<string> v )
{
	tuple_t tuple;
	tuple.srcip = v[0];
	tuple.dstip = v[1];
	tuple.protcol = v[2];
	tuple.srcport = stoi( v[3] );
	tuple.dstport = stoi( v[4] );
	tuple.reach_time = stof( v[5] );

	return tuple;
}

int main( int argc, char ** argv )
{
	/* ---------- $B%A%e!<%K%s%0MQ%Q%i%a%?(B ----------------------------------*/
	// userlistに登録可能な最大user数
	USER_MAX = atoi( argv[4] );
	// flowlistに登録可能な最大flow数
	FLOW_MAX = atoi( argv[5] );
	// thresholdの値
	THRESHOLD = atoi( argv[6] );
	// UserListの初期化時間間隔
	USERLIST_INIT_INTERVAL = atof( argv[7] );
	/*---------------------------------------------------------------------*/
	userlist_init_time = USERLIST_INIT_INTERVAL;

	char bin_tuple[105];
	char ope_str[4];
	strcpy( ope_str, argv[3] );

	string fivetuple, line, str_bintuple, key_string;
	ifstream ifs_r( argv[1] ), ifs( argv[1] );
	vector<string> tmp_vector;
	vector< int > flow_num_per;
	tuple_t tuple;
	int i = 0, index = 0, j = 0, skip = 0, flow_num_per_count = 0, flow_tmp_num = 0;
	double hit_rate = 0.0, int_time = 0.1;

	listInit();
	makeUserList();

	while( getline( ifs_r, line ) )
	{
		tmp_vector = split( line, ' ' );
		tuple = substituteTuple( tmp_vector );
		ump_tuple[tuple.srcip + " " + tuple.dstip + " " + tuple.protcol + " " 
		+ to_string( tuple.srcport) + " " + to_string( tuple.dstport ) ]++;
	}

	cout << "ump created" << endl;
	ifs_r.close();


	while( getline( ifs, line ) )
	{
		tmp_vector = split( line, ' ' );
		tuple = substituteTuple( tmp_vector );
		key_string = tuple.srcip + " " + tuple.dstip + " " + tuple.protcol + " " 
		+ to_string( tuple.srcport ) + " " + to_string( tuple.dstport );
		//5tuple$B$r(B104$B%S%C%H$N(B2$B?J?t$KJQ49$9$k(B
		binaryConvert( tuple, bin_tuple );
		str_bintuple = string( bin_tuple ); 
		index = crcOperation( str_bintuple );
		//cout << index << ":" << str_bintuple << endl;
		//8$B%S%C%H$N%$%s%G%C%/%9$r:n@.(B
//		index = crcOpeforIP( bin_tuple );
//		tmp_black_node = isUserRegistered( tuple );
//		if ( ( tmp_black_node == NULL ) || ( tmp_black_node->isblackuser == 0 ) )
//		{ 
//		printValueIndex( index );	
//		cout << " || " << endl;
//		if ( ump_tuple[key_string] > 1 )
//		{
//			skip++;
		if ( tuple.reach_time > int_time )
		{
			flow_num_per.push_back(flow_num_per_count);
//			if ( flow_num_per_count > 400 )
//			{
//				strcpy( ope_str, "ON" );
//				cout << int_time << ":ON ==>" << endl;
//			}
//			else
//			{
//				strcpy( ope_str, "OFF");
//				cout << int_time << ":OFF <==" << endl;
//			}
			flow_num_per_count = 0;
//			identifyRateCounter();
			int_time = int_time + INTERVAL;
		}
		
//		listOperation( tuple, index, argv[2], ope_str, argv[8] ); 
		listOperation( tuple, index, argv[2], argv[3], argv[8] ); 
//		}
		if ( ump_tuple[key_string] == 1 )
		{
			j++;
		}

	//		printValueIndex( index );
//		printUserList();
//		}
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
		flow_num_per_count++;
		if ( i % 10000 == 0 )
		{
			//cout << i << endl;
		}
	}

//	int j = 0;
//	for ( auto itr = mp_tuple.begin() ; itr != mp_tuple.end() ; itr++ )
//	{
//		if ( itr->second == 1 )
//		{
//			j++;
//			cout << itr->first << endl;
//		}
//	}
//	cout << j << endl;
//	printValue();
//	fclose( inputfile );
//	fprintf( stdout, "input file is closed\n" );
//	flowStaticMain(); //$BF~NO%Q%1%C%H$NE}7W>pJs$r<h$k(B
//	printValueStaticAll();
//
	fprintf( stdout, "flow num:%d\n", ump_tuple.size() );
	fprintf( stdout, "1pflow num:%d\n", j );
	fprintf( stdout, "not 1pflow num:%d\n", skipflow );
	cout << "skip:" << skipflow << " onepflow:" << onepflow << 
	" correct rate:" << 1.0 * onepflow / skipflow << endl;
	hit_rate = 1.0 * (double)hitflag / i;
	fprintf( stdout, "all packet:%d hit:%d miss:%d hit rate:%lf\n", i, hitflag, miss, hit_rate );
	printHitrate();
	double capu_time = 0.0;
//	for ( auto itr = identify_rate.begin() ; itr != identify_rate.end() ; itr++ )
//	{
//		cout << capu_time << ", " << *itr << endl;
		//capu_time += atof( argv[7] );
//		capu_time = capu_time + INTERVAL;
//	}
//
//	for ( auto itr = flow_num_per.begin() ; itr != flow_num_per.end() ; itr++ )
//	{
//		cout << capu_time << ", " << *itr << endl;
//		capu_time = capu_time + INTERVAL;
//	}

	return 0;
}
