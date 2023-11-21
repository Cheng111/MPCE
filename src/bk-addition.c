#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "graph.h"
#include "bk.h"
#include "ReadKG.h"

/* Global Variables */
extern int VERSION;
extern int PRINT;
//int lb1, lb2, lb3;
//int Spart;
extern char * partitefile;
extern int p1_size, p2_size, p3_size;
extern char C1[];
extern char C2[];
extern char C3[];

void clique_find_v7(FILE *fp, u64 *nclique, Graph *G, \
		vid_t *clique, vid_t *old, int lc, int ne, int ce, int * csizes)
{
	int i, j, k;
	GP * gp;
	vid_t u, v;
	FILE *fout = fopen("filename.txt", "w");
	//printGraph(G, fout);
	fclose(fout);
	for(k = 0; k < G->Pnum; k++ )
	{
		gp = G->gps + k;
		for(i = 0; i < gp->size; i++)
		{
			for(j = i + 1; j < gp->size; j++)
			{
				u = old[gp->vertices[i]];
				v = old[gp->vertices[j]];
				//printf("u %d v %d label_u %s label_v %s\n", u, v, G->_label[u], G->_label[v]);
				add_edge(G, u, v);
			}
		}
	}
	//fout = fopen("filename1.txt", "w");
	//printGraph(G, fout);
	//fclose(fout);
	clique_find_v7_sub(fp, nclique, G, clique, old, lc, ne, ce, csizes);
}

void clique_find_v7_sub(FILE *fp, u64 *nclique, Graph *G, \
		vid_t *clique, vid_t *old, int lc, int ne, int ce, int * csizes)
{
  vid_t new[ce];
  int new_ne, new_ce;
  vid_t u, pivot;
  int i, j, pnei;//neibor of pivot vertex should be in P
  //vid_t newvertex[Spart][ce];
  //int new_psizes[G->Pnum];
  int upid;//, jpid;
  int parclique = 0;

  pivot = old[ne];
  pnei = ce -1;
  while (ne < ce) 
  {
	u = old[ne];
	upid = G->_category[u];
	if (u != pivot)
	{
		if(edge_exists(G, u, pivot))
		{
			//ne++;
			//tmpu = u;
			old[ne] = old[pnei];
			old[pnei] = u;
			pnei--;
			//if(pnei < ne)
			//{break;}

			continue;
		}
	}
	if(pnei < ne)
	{break;}
	/* Set new cand and not */
	memset(new, -1, ce*sizeof(vid_t));
	//memset(new_psizes, 0, G->Pnum * sizeof(int));
    new_ne = 0;
	for (j = 0; j < ne; j++)
	{
	  if (edge_exists(G, u, old[j])) 
	  {
		  //jpid = G->_category[old[j]];
		  //newvertex[jpid][new_psizes[jpid]] = old[j];
		  //new_psizes[jpid]++;
		  new[new_ne++] = old[j];
	  }
	}
	new_ce = new_ne;
	for (j = ne+1; j < ce; j++)
	{
	  if (edge_exists(G, u, old[j]))
	  {
		  //jpid = G->_category[old[j]];
		  //new_psizes[jpid]++;
		  new[new_ce++] = old[j];//add N(u) nad P(u)(same partite set) to X
	  }
	}
	
	/* Output clique or extend */
	upid = G->_category[u];
	clique[lc] = u;
	csizes[upid]++;
	if(new_ce == 0) 
	{
	  parclique = 0;
	  //printf("????????\n");
	  for(i = 0; i < G->Pnum; i++)
	  {
		  if(csizes[i] < G->lbs[i])
		  {
			  //printf("%d, %d, %d\n", i, csizes[i], G->lbs[i]);
			  parclique = 1;
			  break;
		  }
	  }
	  if(parclique == 0)
	  {
		  nclique[lc+1]++;
	    //print("hahahah LB %d\n", LB);
	  	if (PRINT) clique_out(fp, G, clique, lc+1);
	  }
	}
	else { 
	  if (new_ne < new_ce)
	  {
	    clique_find_v7_sub(fp, nclique, G, clique, new, lc+1, new_ne, new_ce, csizes);
	  }
	}
	csizes[upid]--;

	
	/* Move u to not */
	ne++;

	/* Bound condition: Stop if any not is a neighbor of all candidates */ 
    for (i = 0; i < ne; i++) {
	  for (j = ne; j < ce; j++) {
	    if (!edge_exists(G, old[i], old[j])) break;
	  }
	  if (j == ce) return;
	}


  }

}