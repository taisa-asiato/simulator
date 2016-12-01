#ifndef _DEFINE_H
#define _DEFINE_H
/* header file */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ENTRY_MAX 1024
#define WAY_MAX 4 //way数の最大値

#define EQUAL 1
#define NOTEQUAL -1

///////////////////////////////
/* 5タプルの情報を持つ構造体 */
///////////////////////////////
typedef struct _tapple
{
	char srcip[17];
	char dstip[17];
	int srcport;
	int dstport;
	char protcol[4];
	double reach_time;
} tapple_t;

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
	tapple_t entry;
	double diff_of_time;
	int flow_interval;
	int search_flag;
	time_interval_t * time_relative;
	struct _node * prev;
} node_t;

typedef struct _another_node
{
	struct _another_node * next;
	tapple_t entry_another;
} another_node_t;

//////////////////////
/* プロトタイプ宣言 */
//////////////////////
void listInit();
void listInsert( tapple_t x, int number );
tapple_t stringSplit( char * tapple_string );
void listOperation( tapple_t x, int index );
node_t * isRegistered( tapple_t inputTapple, int index );
int isEqual( tapple_t inputTapple, node_t * node );
void listDeleteFirst( int index );
void printValue();
void listSubstitute( node_t * pointer, tapple_t x );
void binaryConvert( tapple_t x, char * bin_tapple );
int crcOperation( char * bin_tapple );
void lruPolicy( tapple_t x, int index );
void spPolicy( tapple_t x, int index );

//統計情報を取るための関数本体
void flowStatic();
 //統計情報を取るためのリストに要素の追加を行う関数
void listInsertStatic( tapple_t x, int number );
 //統計情報を取るためのリストの初期化を行う関数
void listInitStatic();
//numberで指定されたインデックスからdelete_pointerを削除する
node_t * listDeleteStatic( node_t * delete_pointer, int number );
//統計情報を取るためのリストから, 入力のsearch_pointerと等しいノードを探す関数
int listSearchStatic( node_t * search_pointer, int number );
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
void anotherListInsert( tapple_t x, another_node_t * pointer );
//仮のリストに要素が追加されているかどうか確認する関数, isRegisteredを改変したもの
another_node_t * isRegisteredStaticList( tapple_t inputTapple, another_node_t * pointer );
//isEqualを改変したもの, 第二引数の型が違う
int isEqualStaticList( tapple_t inputTapple, another_node_t * pointer );
//仮のリストの初期化を行う関数
void anotherListInit( another_node_t * pointer );
//仮のリストの要素を全て削除する関数
void deleteAnotherList( another_node_t * pointer );
//仮のリストの要素を出力する関数, デバッグ用
void printAnotherList( another_node_t * pointer );

////////////////////
/* グローバル変数 */
////////////////////
FILE *inputfile; //入力ファイルを指すファイルポインタ
extern int entry_size; //現在のエントリ数を指す
extern int INDEX_MAX; //インデックスの最大数を示す
extern int hitflag; //エントリ中でヒットした回数
extern int miss; //エントリ中でミスした回数

node_t * head[ENTRY_MAX / WAY_MAX]; //最初のエントリを指すポインタ
node_t * p[ENTRY_MAX / WAY_MAX]; //エントリの最後を指すポインタ

node_t * head_static[ENTRY_MAX / WAY_MAX]; //統計情報を取るために用いるリストの最初のエントリを指すポインタ配列
node_t * p_static[ENTRY_MAX / WAY_MAX]; //上記のリストのエントリの最後を指すポインタ配列
#endif
