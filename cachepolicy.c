#include "define.h"

/* LRU$B%]%j%7!<(B. $B0lItJ#?t%]%j%7!<$GE}0l$G$-$kItJ,$,$"$k(B */
void lruPolicy( tuple_t x, int index )
{
	node_t * tmp;
	if ( ( tmp = isRegistered( x, index ) ) != NULL )
	{ 	//list$BCf$KEPO?$5$l$F$$$k>l9g(B
		hitOrMiss( x, 1 );
		if ( tmp == p[index] )
		{	/* $B@hF,%N!<%I$K$"$k>l9g$K$OM%@hEY$,0lHV9b$$;v$r<($7$F$$$k$N$G2?$b$;$:$K=*N;(B */
			;
		}
		else if ( tmp != p[index] )
		{
			if( tmp == head[index]->next )
			{
				/* $B:G=*%N!<%I$K$"$k>l9g(B */
				head[index]->next = tmp->next; // tmp->next$B$N%N!<%I$,:GDc%N!<%I$K$J$k$?$a(B, head[index]->next$B$G$=$N%N!<%I$r;X$9(B
				tmp->next->prev = head[index]; // tmp->next$B$N%N!<%I$,:GDc%N!<%I$N$?$a(B, $B$3$N%N!<%I$NA0$N%N!<%I$O(Bhead[index]$B$H$J$k(B
			}
			else if ( tmp != head[index]->next )
			{
				/* $B:G=*%N!<%I$G$J$$>l9g(B */
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
			}

			p[index]->next = tmp; //LRU$B$J$N$G(B, $B0lHVM%@hEY$N9b$$%(%s%H%j$KDI2C(B
			tmp->next = NULL;
			tmp->prev = p[index];
			p[index] = tmp;
			listSubstitute( tmp, x );

		}
	}
	else
	{	//list$BCf$K(B5$B%?%W%k$,EPO?$5$l$F$$$J>l9g(B
		hitOrMiss( x, 0 );
		//list$BCf$KEPO?$5$l$F$$$J$$>l9g$K$O(B, $BM%@hEY$NDc$$%(%s%H%j$r:o=|$7$?8e(B,$B!!0lHVM%@hEY$N9b$$>l=j$K%(%s%H%j$rEPO?$7D>$9(B
		listDeleteFirst( index );
		listInsert( x, index );
	}
}

/* SP$B%]%j%7!<(B */
void spPolicy( tuple_t x, int index )
{
	node_t * tmp;
	node_t * tmp_next;

	if ( ( tmp = isRegistered( x, index ) ) != NULL )
	{	// list$BCf$KEPO?$5$l$F$$$k>l9g(B
		hitOrMiss( x, 1 );
		if ( tmp == p[index] )
		{	/* $B@hF,%N!<%I$K$"$k>l9g$K$OM%@hEY$,0lHV9b$$;v$r<($7$F$*$j2?$b$;$:$K=*N;(B */
			;
		}
		else if ( tmp != p[index] )
		{
			if ( tmp->next != p[index] )
			{
				/* $B$d$C$F$k;v$ONY$N%N!<%I$H(Btmp$B$N%N!<%I$NF~$lBX$((B 
				 * $B$9$4$/FI$_$E$i$$$N$G(B, $BIaDL$K(Bswap$B4X?t$H$+$r:n$k$Y$-$@$C$?(B */
				tmp_next = tmp->next->next;
				tmp_next->prev = tmp;
				tmp->next->prev = tmp->prev;
				tmp->prev->next = tmp->next;
				tmp->prev = tmp->next;
				tmp->prev->next = tmp;
				tmp->next = tmp_next;
			}
			else if ( tmp->next == p[index] )
			{
				p[index]->next = tmp;
				p[index]->prev = tmp->prev;
				tmp->next = NULL;
				tmp->prev->next = p[index];
				tmp->prev = p[index];
				p[index] = tmp;
			}
		}

	}
	else 
	{	// list$BCf$KEPO?$5$l$F$$$J$$>l9g(B
		hitOrMiss( x, 0 );
		/* $B0lHVM%@hEY$NDc$$%(%s%H%j$KEPO?$9$k(B */
		listSubstitute( head[index]->next, x );
	}
}
