#ifndef _DEFINE_H
#define _DEFINE_H
#define _GLIBCXX_DEBUG // デバッグ文出力
/* header file */
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <omp.h>
#include <err.h>
#include <algorithm>

#define ENTRY_MAX 1024
#define INDEX_MAX 256 
//way数の最大値

#define EQUAL 1
#define NOTEQUAL -1

#define ONLY_SEARCH 1
#define SEARCH_FIND 2

/* ---------- チューニング用パラメタ ----------------------------------*/
// ブラックリストに登録できる最大のuser数				
//#define USER_MAX 100
// ブラックリストに登録された各userの生成したフローの最大登録数		
//#define FLOW_MAX 10
// ブラックリストに登録されているuserの生成したflowのパケット数の閾値	
//#define THRESHOLD 1
// BlackListの初期化間隔						
//#define BLACKLIST_INIT_INTERVAL 0.01
// Blackuserの初期化間隔
#define BLACKUSER_INIT_INTERVAL 1.0
// INTERVAL時間
#define INTERVAL 0.01
/*---------------------------------------------------------------------*/

///////////////////////////////
/* 5タプルの情報を持つ構造体 */
///////////////////////////////
typedef struct _tuple
{
	std::string srcip;
	std::string dstip;
	int srcport;
	int dstport;
	std::string protcol;
	double reach_time;
	int datasize;
} tuple_t;

/////////////////////////////////////////
/* 5タプルの情報を持つ構造体(時刻無し) */
/////////////////////////////////////////
typedef struct _tuplelst
{
	std::string srcip;
	std::string dstip;
	int srcport;
	int dstport;
	std::string protcol;
} tuplelst_t;


//////////////////////////////
/* 時間間隔を保持するリスト */
//////////////////////////////
typedef struct _node_time_interval
{
	struct _node_time_interval * next;
	double time_interval;
} time_interval_t;

//////////////////////////
/* 双方向リストの構造体 */
//////////////////////////
typedef struct _node
{
	struct _node * next;
	tuple_t entry;
	double diff_of_time;
	double diff_of_crc_time;
	int flow_interval;
	int search_flag;
	int crcnum;
	int crc_flow_interval;
	time_interval_t * time_relative;
	struct _node * prev;
} node_t;

////////////////////////////////////////
/* 統計情報を記録しておくためのリスト */
////////////////////////////////////////
typedef struct _another_node
{
	struct _another_node * next;
	node_t * pointer_for_listStatic;
	tuple_t entry_another;
	int packet_count;
} another_node_t;

//////////////////////////////////////////
/* userが生成したフローを保持するリスト */
//////////////////////////////////////////
typedef struct _sent_flow
{
	// 次の要素を指すポインタ
	struct _sent_flow * next;
	// 前の要素を指すポインタ
	struct _sent_flow * prev;
	// このフローが何パケットのフローかを示す
	int count;
	// TODO:今は5タプルの値を登録するが, メモリの容量を考えると別の方が好ましい
	tuple_t flowid;
} sent_flow_t;

//////////////////////////////
/* ブラックリストの先頭部分 */
//////////////////////////////
typedef struct _user_list
{
	// この構造体は, 要素として
	// 送信元IP ( userip ), フローを登録するブラックリストを保持する
	std::string userip;
	// ブラックリストの先頭要素のアドレスを指すポインタ
	sent_flow_t * blacksentflow;
	sent_flow_t * blacksentflow_end;
	// 次のblack_head_tの要素を指すポインタ
	struct _user_list * next;
	// 前のノードを指すポインタ 
	struct _user_list * prev;
	// flowの数
	std::map< std::string, sent_flow_t * > ump_sentflow;
	//std::unordered_map< std::string, sent_flow_t * > ump_sentflow;

	int flow_number;
	int onepacket_number;
	int isblackuser;
	double registered_time;
} user_list_t;

///////////////////////////////////////
/* unordered_mapで構成されたUserList */
///////////////////////////////////////
typedef struct _ump_user
{
	// 送信元IP ( userip ), フローを登録するブラックリストを保持する
	std::string userip;
	// 送信フローのリスト
	std::list< sent_flow_t > sentflow;
	std::unordered_map< std::string, std::list< sent_flow_t >::iterator > ump_sentflow;
	int flow_number;
	int onepacket_number;
	int isblackuser;
	double registered_time;
} ump_user_t;

//////////////////////
/* プロトタイプ宣言 */
//////////////////////
// TODO 同じような機能の関数がたくさんあるので, 統合すべき
// TODO ファイル毎に関数をソートすべき
/* list.cで宣言されている関数群 */
// リストの初期化を行う関数
void listInit();
// リストの挿入を行う関数
void listInsert( tuple_t x, int number );
void listNodeInit( node_t * pointer );
// hit, missの回数をカウントする関数, 
void hitOrMiss( tuple_t tuple, int isHit );
// 単位時間あたりのhit率を測定する
void hitRatePerSec( double reach_time );
// リストの内容を出力する関数;
void printValue();
void printValueIndex( int index );
// inputTupltとリストのnodeのタプルが一致するかどうかを確認する関数
int isEqual( tuple_t inputTapple, node_t * node );
// inputTupleがリストの登録されているか確認する関数
node_t * isRegistered( tuple_t inputTapple, int index );
// リストのエントリ操作の大本をおこなう関数,
void listOperation( tuple_t x, int index, char * operation, char * blacklist, char * debug );
// BlackListアリでキャッシュのエントリ置換を行う場合
void listOperationWithList( tuple_t x, int index, char * operation, char * debug );
// BlackListなしでキャッシュのエントリ置換を行う場合
void listOperationNoList( tuple_t x, int index, char * operation, char * debug );
// キャッシュのエントリの置換ポリシーの切り替えを行う
void switchPolisy( tuple_t x, int index, char * operation, node_t * tmp );
tuple_t stringSplit( char * tuple_string );
void listDeleteFirst( int index );
void listSubstitute( node_t * pointer, tuple_t x );
void binaryConvert( tuple_t x, char * bin_tuple );
int crcOperation( std::string bin_tuple );
int crcOpeforIP( std::string bin_tuple );
void printValueCRC( char * crc, char * tmp, int position );
tuple_t initializeTuple();
void listExchange( int index );
std::array< node_t * , 4 > regEntry( int index );
node_t *  isUserHasEntry( user_list_t * tmp, std::array< node_t *, 4 > reg_entry );
void moveToMPState( node_t * tmp, int index );
void initBlackUserEntry( node_t * tmp_entry, int index );



std::vector< std::string > split( const std::string str, char sep );

/* cachepolicy.cで宣言されている関数群 */
void lruPolicy( tuple_t x, int index, node_t * tmp );
void spPolicy( tuple_t x, int index, node_t * tmp );
void getInputFileRow( char * filename );
// 1秒辺りのヒット率を出力する関数
void printHitrate();
void printTcamRate();
void printTcamACCRate();
void printFlowNum();
void printHitratesec();
void printNTcamRate();
void printNTcamACCRate();
void printDataPS();

//統計情報を取るための関数本体
void flowStaticMain();
//解析を行う関数, 逐次処理を行う
void flowStaticForSingle();
//解析を行う関数, 並列処理を行う
void flowStaticForParallel();
 //統計情報を取るためのリストに要素の追加を行う関数
int listInsertStatic( node_t * end, tuple_t x, int number );
 //統計情報を取るためのリストの初期化を行う関数
void listInitStatic();
//numberで指定されたインデックスからdelete_pointerを削除する
node_t * listDeleteStatic( node_t * delete_pointer, int number );
//統計情報を取るためのリストから, 入力のsearch_pointerと等しいノードを探す関数
void listSearchStatic( tuple_t search_tuple, int number );
//統計情報を出力する関数
void printValueStatic( node_t * pointer, int number );
//統計情報のリストの要素を全て出力する関数
void printValueStaticAll();
//パケットの到着間隔を保持するリストの初期化を行う関数
void TimeListInit( node_t * pointer );
//パケットの到着間隔を保持するリストに要素を追加する関数
void TimeListInsert( node_t * pointer, double interval );
//パケットの到着間隔を保持するリストの要素を出力する関数
void printTimeRelative( node_t * pointer );
//全パケットを保持するリストの各要素に対して, flow_intervalとdiff_of_timeの初期化を行う関数
void listStaticSubstitute( node_t * node );
//全パケットを保持するリストの要素に対して, 処理済みの要素を削除する関数, 主にメモリ容量が足りないときに使用
node_t * freeListStatitc( node_t * pointer );
void printAttackerIP();


//フロー間の要素の測定を置こなう際にフローの要素を登録する仮のリストに要素を追加する関数
void anotherListInsert( node_t * staticnode, another_node_t * pointer );
//仮のリストに要素が追加されているかどうか確認する関数, isRegisteredを改変したもの
another_node_t * isRegisteredStaticList( tuple_t inputTuple, another_node_t * pointer );
//isEqualを改変したもの, 第二引数の型が違う
int isEqualStaticList( tuple_t inputTuple, another_node_t * pointer );
//仮のリストの初期化を行う関数
void anotherListInit( another_node_t * pointer );
//仮のリストの要素を全て削除する関数
void deleteAnotherList( another_node_t * pointer );
//仮のリストの要素を出力する関数, デバッグ用
void printAnotherList( another_node_t * pointer );
//仮のリストの要素を削除しつつ, 統計用リストの要素の情報のアップデートも行う
void deleteAnotherListAndUpdate( another_node_t * pointer );

/* filter.c */
// ブラックリストの初期化を行う関数
void userListInit();
// ブラックリストへの各種操作を行う関数
int userListOperation( tuple_t tuple );
// フローを生成しているuserがリストに登録されているかどうか確認する関数
user_list_t * isUserRegistered( tuple_t tuple );
// ブラックリストのそれぞれのノードを作成する関数
int makeUserList();
// ブラックリストに登録されているuserが生成したフローとそのパケット数を出力する
void printUserList( user_list_t * pointer_to_userlist );
// flowを記録するノードの初期化を行う関数
void initializeFlowList( sent_flow_t * flow_node );
// ブラックリストの登録されたuserが生成したflowが登録されているかを確認する
sent_flow_t * isFlowRegistered( user_list_t * node, tuple_t tuple );
// ブラックリストに登録するuserのリストの初期化を行う関数
void initializeUserList( user_list_t * user_node );
// ブラクリストに登録されたuserが保持するフローリストからフローを削除する
int removeFlow( sent_flow_t * remove_node, user_list_t * user_node );
// ブラックリストに登録されたuserを削除する
void removeUser( user_list_t * user_node );
// ブラックリストに登録されたuserの優先順位を変更する関数
void blackListSwap( user_list_t * user_node );

// ブラックリストにuserを登録(追加する)
void addUser( tuple_t tuple );
// ブラックリストにuserを登録する
int substituteUser( user_list_t * tmp, tuple_t tuple );
// フローリストにフローを追加する
int substituteFlow( sent_flow_t * flow_node, tuple_t tuple  );
user_list_t * registUser( tuple_t tuple );
sent_flow_t * addFlow( user_list_t * user_node );
void printBlackUser( user_list_t * pointer_to_userlist );
void printUserList( user_list_t * pointer_to_userlist );
void printUserListReverse();
void printSentFlow( user_list_t * user_node );
void printRegisteredBlackList();
int makeFlowList( user_list_t * user_node );
int deleteFlow( sent_flow_t * flow_node );
sent_flow_t * deleteLastFlowNode( sent_flow_t * flow_node );
void mallocFailed();
void swapUserNode( user_list_t * user_node );
void initializeAllFlowList( sent_flow_t * flow_node );
void newUserForMaxList();
sent_flow_t * moveLastFlowNode( sent_flow_t * flow_node, user_list_t * user_node ); 
void userListNodeInit( user_list_t * user_ndoe );
void moveLastUserNode( user_list_t * user_node ); 
void userListIntervalInit();
void userListIntervalInitAll();
void blackuserIntervalInit( double reach );
void identifyRateCounter();

/////////////////
/* comflow/cpp */
/////////////////
int isSimilarFlow( user_list_t * tmp_user, tuple_t tuple );
int ump_isSimilarFlow( tuple_t tuple );

////////////////////
/* ump_filter.cpp */
////////////////////
int ump_UserListOperation( tuple_t tuple );
std::list< ump_user_t >::iterator ump_isUserRegistered( tuple_t tuple );
void ump_registUser( tuple_t tuple );
ump_user_t ump_initialUserNode( tuple_t tuple );
void ump_substituteUser( user_list_t * tmp, tuple_t tuple );
void ump_initSentFlowList( std::string str_userip );
void ump_deleteUserListLastNode();
void ump_deleteFlowListLastNode( tuple_t tuple );
void ump_registFlow( tuple_t tuple );
/*std::unordered_map< 
	std::string, 
	std::list< ump_user_t >::iterator 
>::iterator*/
void ump_moveFirstNode( tuple_t tuple );
void ump_initUserList();
void ump_userListIntervalInitAll();
void ump_userListIntervalInit();
void ump_blackuserListIntervalInit( double now_time );
void ump_printUserList();
void ump_printSentFlow( std::list< sent_flow_t > tmp_sent_flow );
void ump_printSentFlowReverse( std::list< sent_flow_t > tmp_sent_flow );

/////////////
/* opt.cpp */
/////////////
void OPT( tuple_t tuple, int index, node_t * tmp );

///////////////////
/* misscheck.cpp */
///////////////////
int FullAsoHitOrMiss( tuple_t tuple );
int missCharacterCheck_FullAso( tuple_t tuple, int isHit );
void UpdateFullAsoCache( tuple_t tuple, int f_ishit );
void ListCacheOperationMain( tuple_t tuple, int index, int isHit );
void UpdateCache( std::string key_string, int index, std::list< std::string >::iterator itr );
std::list< std::string >::iterator LC_IsRegistered( std::string key_string, int index );

////////////////////
/* グローバル変数 */
////////////////////

/*----------チューニング用パラメータ----------*/
// ブラックリストに登録できる最大のuser数
extern int 	USER_MAX;
// ブラックリストに登録された各userの生成したフローの最大登録数		
extern int	FLOW_MAX;
// ブラックリストに登録されているuserの生成したflowのパケット数の閾値	
extern int	THRESHOLD;
// ユーザーリストに登録されているuserがエレファントユーザであるときの閾値
extern int 	ELE_THRESHOLD;
// UserListの初期化間隔						
extern double	USERLIST_INIT_INTERVAL;
/*==========================================*/

extern int 	user_number;
extern int 	entry_size; //現在のエントリ数を指す
extern int 	WAY_MAX; //インデックスの最大数を示す
extern int 	hitflag; //エントリ中でヒットした回数
extern int 	miss; //エントリ中でミスした回数
extern int 	hit_per_sec; // 1秒あたりのヒット数
extern int 	miss_per_sec; // 1秒辺りのミス数
extern double 	arrival_time; // パケットの到着時刻を示す
extern std::list< double > hitrate_per_sec; // 1秒あたりのヒット率を記録する
extern std::list< double > tcamrate_per_sec; // 1秒あたりのtcamアクセス率を記録する
extern std::list< int > tcam_per_sec;
extern double 	userlist_init_time; // 一定時間ごとにブラックリストを初期化するための時間を保持する
extern unsigned int 	filerow;
extern int	skipflow;
extern int 	onepflow;
extern node_t * head[INDEX_MAX]; //最初のエントリを指すポインタ
extern node_t * p[INDEX_MAX]; //エントリの最後を指すポインタ
extern std::unordered_map< std::string, int > ump_tuple; // フローとそのパケット数を記録する連想配列
extern std::unordered_map< std::string, std::list< int > > opt_list;
extern std::vector< double > identify_rate;
//extern std::map< std::string, std::list< ump_user_t >::iterator > ump_userlist;
extern std::unordered_map< std::string, std::list< ump_user_t >::iterator > ump_userlist;
extern std::unordered_map< std::string, std::list< ump_user_t >::iterator > ump_blackuser;
extern double hit_1p;
extern double skip_1p;
// node_t * head_static[INDEX_MAX]; //統計情報を取るために用いるリストの最初のエントリを指すポインタ配列
// node_t * p_static[INDEX_MAX]; //上記のリストのエントリの最後を指すポインタ配列
//本来の情報を登録するリスト
// node_t * analyze;
// node_t * analyze_end;

//検索用のリスト
// node_t * search;
// node_t * search_end;
// extern node_t * static_search;
// exteggrn node_t * search_end;
//仮のリストの先頭要素を保持するポインタ配列
//another_node_t * another_tmp_list[ENTRY_MAX / WAY_MAX];
// ブラックリスト, キャッシュエントリに登録しないフローを生成するuserを登録する
extern user_list_t * userlist;
extern user_list_t * userlist_end;
// unordered_mapを用いたuserlistの先頭および末端エントリを指すリスト
extern std::list< ump_user_t > ump_l_userlist;
//user_list_t blackuser[100];

/* For Full Asociative Cache */
extern std::list< std::string > cache_fulaso;
extern std::unordered_map< std::string, int > issent;
extern int first_miss;
extern int conflict_miss;
extern int capacity_miss;
extern std::array< std::list< std::string >, 256 > lst_cache;

// キャッシュにヒット/ミスした場合 ( 1でヒット, 0でミス )
extern int HITORMISS;

// listを用いたOPTアルゴリズムのキャッシュのミス/ヒットをカウントする
extern int OPTHIT;
extern int OPTMISS;

// TCAMの単位時間あたりのアクセス数
extern 	int 	tnaccess_per_sec;
// 
extern std::list< int > flownum_per_sec;
extern std::unordered_map< std::string, int > flownum;
extern std::unordered_map< std::string, int > attacker;

extern std::list< double > ntcamrate_per_sec;
extern std::list< int > ntcam_per_sec;
extern double NIDS_RATE;
extern double skipdatasize;
extern std::list< double > nids_senddatasize;

extern 	int 	taccess_per_sec;
extern std::list< int > tacc_per_sec;

extern std::array< std::unordered_map< std::string, std::unordered_map< std::string, int > >, 903 > user_interval;
extern int average_flownum;
#endif
