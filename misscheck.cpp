/* This File contains 
 * 1. Full Asociative Cache Operation
 * 2. Check Miss Character 
 * */

#include "define.h"

using namespace std;

int FullAsoHitOrMiss( tuple_t tuple )
{
	string key_string;
	key_string = tuple.srcip + " " + tuple.dstip + " " + tuple.protcol + 
		" " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );

	auto itr = find( cache_fulaso.begin(), cache_fulaso.end(), key_string );
	if ( itr != cache_fulaso.end() )
	{	// フルアソキャッシュでヒットする場合
		return 1;
	}

	// フルアソでヒットしなかった場合
	return 0;
}

int missCharacterCheck( tuple_t tuple, int isHit )
{
	string key_string;
	key_string = tuple.srcip + " " + tuple.dstip + " " + tuple.protcol + 
		" " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );



	int f_ishit = FullAsoHitOrMiss( tuple );

	if ( isHit == 0 )
	{
		if ( f_ishit == 1 )
		{	// フルアソでヒットしたが, 普通のキャッシュではヒットしなかった場合
			conflict_miss++;
		}
		else if ( f_ishit == 0 )
		{
			auto itr = issent.find( key_string );
			if ( itr != issent.end() )
			{	// 以前に送信済みパケットである場合
				capacity_miss++;
			}
			else 
			{	// 初めて参照されたパケットである場合
				first_miss++;
			}
		}
	}


	UpdateFullAsoCache( tuple, f_ishit );
}

void UpdateFullAsoCache( tuple_t tuple, int f_ishit )
{
	string key_string;
	key_string = tuple.srcip + " " + tuple.dstip + " " + tuple.protcol + 
		" " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );


	issent[key_string] = 1;

	/* フルアソのキャッシュ置換アルゴリズムもRLUを使用する */
	if ( f_ishit == 1 )
	{	// フルアソのキャッシュにヒットした場合
		auto itr = find( cache_fulaso.begin(), cache_fulaso.end(), key_string );
		cache_fulaso.erase( itr );
		cache_fulaso.push_front( key_string );
	}
	else if ( f_ishit == 0 )
	{	// フルアソのキャッシュにヒットしなかった場合
		int f_size = cache_fulaso.size();
		if ( f_size < ENTRY_MAX )
		{
			cache_fulaso.push_front( key_string );
		}
		else 
		{
			cache_fulaso.pop_back();
			cache_fulaso.push_front( key_string );
		}
	}
}
