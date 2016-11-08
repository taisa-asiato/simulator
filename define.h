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
} tapple_t;

//////////////////////////
/* 双方向リストの構造体 */
//////////////////////////
typedef struct _node
{
	struct _node * next;
	tapple_t entry;
	struct _node * prev;
} node_t;

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
