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

#define EQUAL 1;
#define NOTEQUAL -1;

/* 5タプルの情報を持つ構造体 */
typedef struct _tapple
{
	char srcip[17];
	char dstip[17];
	int srcport;
	int dstport;
	char protcol[4];
} tapple_t;

/* 双方向リストの構造体 */
typedef struct _node
{
	struct _node * next;
	tapple_t entry;
	struct _node * prev;
} node_t;

/* プロトタイプ宣言 */
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

/* グローバル変数 */
FILE *inputfile; //入力ファイルを指すファイルポインタ
extern int entry_size; //現在のエントリ数を指す
extern int INDEX_MAX; //インデックスの最大数を示す
node_t * head[ENTRY_MAX / WAY_MAX]; //最初のエントリを指すポインタ
node_t * p[ENTRY_MAX / WAY_MAX]; //エントリの最後を指すポインタ

#endif
