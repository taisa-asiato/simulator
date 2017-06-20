#ifndef _DEFINE_H
#define _DEFINE_H
/* header file */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <omp.h>
#include <err.h>

#define ENTRY_MAX 1024
#define WAY_MAX 4 //way数の最大値

#define EQUAL 1
#define NOTEQUAL -1

#define ONLY_SEARCH 1
#define SEARCH_FIND 2

// ブラックリストに登録できる最大のuser数
#define BLACKUSER_MAX 100
// ブラックリストに登録された各userの生成したフローの最大登録数
#define FLOW_MAX 5
// ブラックリストに登録されているuserの生成したflowのパケット数の閾値
#define THRESHOLD 2

///////////////////////////////
/* 5タプルの情報を持つ構造体 */
///////////////////////////////
typedef struct _tuple
{
	char srcip[17];
	char dstip[17];
	int srcport;
	int dstport;
	char protcol[4];
	double reach_time;
} tuple_t;

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
typedef struct _black_list
{
	// この構造体は, 要素として
	// 送信元IP ( userip ), フローを登録するブラックリストを保持する
	char userip[17];
	// ブラックリストの先頭要素のアドレスを指すポインタ
	sent_flow_t * blacksentflow;
	// 次のblack_head_tの要素を指すポインタ
	struct _black_list * next;
	// 前のノードを指すポインタ 
	struct _black_list * prev;
	// flowの数
	int flow_number;
	int onepacket_number;
	int isblackuser;
} black_list_t;

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
// hit, missの回数をカウントする関数, 
void hitOrMiss( tuple_t tuple, int isHit );
// リストの内容を出力する関数
void printValue();
// inputTupltとリストのnodeのタプルが一致するかどうかを確認する関数
int isEqual( tuple_t inputTapple, node_t * node );
// inputTupleがリストの登録されているか確認する関数
node_t * isRegistered( tuple_t inputTapple, int index );
// リストのエントリ操作の大本をおこなう関数, ポリシーを切り替える
void listOperation( tuple_t x, int index, char argv[2] );

tuple_t stringSplit( char * tuple_string );
void listDeleteFirst( int index );
void listSubstitute( node_t * pointer, tuple_t x );
void binaryConvert( tuple_t x, char * bin_tuple );
int crcOperation( char * bin_tuple );
int crcOpeforIP( char * bin_tuple );
void printValueCRC( char * crc, char * tmp, int position );

/* cachepolicy.cで宣言されている関数群 */
void lruPolicy( tuple_t x, int index );
void spPolicy( tuple_t x, int index );
void getInputFileRow( char * filename );
// 1秒辺りのヒット率を出力する関数
void printHitrate();


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
void blackListInit();
// ブラックリストへの各種操作を行う関数
int blackListOperation( tuple_t tuple );
// フローを生成しているuserがリストに登録されているかどうか確認する関数
black_list_t * isUserRegistered( tuple_t tuple );
// ブラックリストのそれぞれのノードを作成する関数
int makeBlackList();
// ブラックリストに登録されているuserが生成したフローとそのパケット数を出力する
void printBlackList();
// flowを記録するノードの初期化を行う関数
void initializeFlowList( sent_flow_t * flow_node );
// ブラックリストの登録されたuserが生成したflowが登録されているかを確認する
int isFlowRegistered( black_list_t * node, tuple_t tuple );
// ブラックリストに登録するuserのリストの初期化を行う関数
void initializeBlackUserList( black_list_t * user_node );
// ブラクリストに登録されたuserが保持するフローリストからフローを削除する
int removeFlow( sent_flow_t * remove_node, black_list_t * user_node );
// ブラックリストに登録されたuserを削除する
void removeUser( black_list_t * user_node );
// ブラックリストに登録されたuserの優先順位を変更する関数
void blackListSwap( black_list_t * user_node );
// ブラックリストにuserを登録(追加する)
void addUser( tuple_t tuple );
// ブラックリストにuserを登録する
int substituteUser( black_list_t * tmp, tuple_t tuple );
// フローリストにフローを追加する
int substituteFlow( sent_flow_t * flow_node, tuple_t tuple  );
black_list_t * registUser( tuple_t tuple );
sent_flow_t * addFlow( black_list_t * user_node );
void printBlackList();
void printBlackListReverse();
void printSentFlow();
void printRegisteredBlackList();
int makeFlowList();
int deleteFlow( sent_flow_t * flow_node );
sent_flow_t * deleteLastFlowNode( sent_flow_t * flow_node );
void mallocFailed();
void swapBlackNode( black_list_t * user_node );

////////////////////
/* グローバル変数 */
////////////////////
FILE *inputfile; //入力ファイルを指すファイルポインタ
extern int user_number;
extern int entry_size; //現在のエントリ数を指す
extern int INDEX_MAX; //インデックスの最大数を示す
extern int hitflag; //エントリ中でヒットした回数
extern int miss; //エントリ中でミスした回数
extern int hit_per_sec; // 1秒あたりのヒット数
extern int miss_per_sec; // 1秒辺りのミス数
extern double time; // パケットの到着時刻を示す
extern double hitrate_per_sec[901]; // 1秒あたりのヒット率を記録する
extern double black_time; // 一定時間ごとにブラックリストを初期化するための時間を保持する
extern unsigned int filerow;
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
//black_list_t blackuser[100];
#endif
