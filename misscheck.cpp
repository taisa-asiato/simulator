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

int missCharacterCheck_FullAso( tuple_t tuple, int isHit )
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

std::list< string>::iterator LC_IsRegistered( string key_string, int index )
{
	auto itr = find( lst_cache[index].begin(), lst_cache[index].end(), key_string );
	return itr;
}

void UpdateCache( string key_string, int index, std::list< string >::iterator itr )
{
	int flownum = 0;
	std::list< string >::iterator target_itr;

	if ( itr != lst_cache[index].end() )
	{	// フローがキャッシュに登録済みの場合
		; // 何もしない	
	}
	else 
	{	// フローがキャッシュに登録済みでない場合
		if ( lst_cache[index].size() < ENTRY_MAX/INDEX_MAX )
		{	// 空きエントリがある場合
			lst_cache[index].push_front( key_string );
		}
		else 
		{	// 空きエントリがない場合
			for ( auto tmp_itr = lst_cache[index].begin(); tmp_itr != lst_cache[index].end() ; tmp_itr++ )
			{	// すべてのエントリのうち, 次パケットが来ないもの
				// または時間が一番かかるものをエントリから削除
				auto opt_itr = opt_list.find( *tmp_itr );
				if ( opt_itr->second.size() == 0 )
				{	// パケットが来ないフローのエントリがある場合
					target_itr = tmp_itr;
					break;
				}
				else 
				{	// 次のパケットが来るフローのエントリの場合
					auto list_itr = opt_itr->second.begin();
					if ( *list_itr > flownum )
					{	// 次回到着までに一番時間がかかるフローを探索
						target_itr = tmp_itr;
						flownum = *list_itr;
					}
				}
			}
			lst_cache[index].erase( target_itr );
			lst_cache[index].push_front( key_string );
		}
	}
	opt_list[key_string].pop_front();	
}

void ListCacheOperationMain( tuple_t tuple, int index, int isHit )
{
	string key_string;
	key_string = tuple.srcip + " " + tuple.dstip + " " + tuple.protcol + 
		" " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );
	int f_ishit = 0;

	// Cacheに登録済みのフローかどうか確認
	auto itr = LC_IsRegistered( key_string, index );
	if ( itr != lst_cache[index].end() )
	{	// Cacheに登録済みのフローの場合
		f_ishit = 1;
		OPTHIT++;
	}
	else
	{
		OPTMISS++;
	}


	if ( isHit == 0 )
	{
		if ( f_ishit == 1 )
		{	// OPTでヒットしたが, 通常のキャッシュではヒットしなかった場合
			conflict_miss++;
		}
		else if ( f_ishit == 0 )
		{	// 
			auto itr1 = issent.find( key_string );
			if ( itr1 != issent.end() )
			{	// 以前に送信済みパケットである場合
				capacity_miss++;
			}
			else 
			{	// 初めて参照されたパケットである場合
				first_miss++;
			}
		}
	}

	UpdateCache( key_string, index, itr );
	issent[key_string] = 1;
}
