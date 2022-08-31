/* Enumerate clique by Bron-Kerbosch Algorithms 
 * Author: Cheng Chen Yun Zhang
 * Date: 2022
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "graph.h"
#include "bk.h"
//#include "PartiteSize.h"


/* Global Variables */
int LB, UB, PART;
int VERSION;
int PRINT;
//int lb1, lb2, lb3;
//int Spart;
char * partitefile;
extern int p1_size, p2_size, p3_size;
extern char C1[];
extern char C2[];
extern char C3[];


/* ------------------------------------------------------------- *
 * Function: clique_out()                                        *
 * ------------------------------------------------------------- */
void clique_out(FILE *fp, Graph *G, vid_t *clique, int len)
{
  int i;

  /*if(PART)
  {
	  int p1, p2, p3;
  	  p1 = p2 = p3 =0;
	  //char * PrintCli[len];
	  for (i = 0; i < len; i++)
	  {
		  //if(strcmp(G->_category[clique[i]], C1) == 0)
		  if(G->_category[clique[i]] == 0)
		  {p1 = p1 + 1;}
		  //else if(strcmp(G->_category[clique[i]], C2) == 0)
		  else if(G->_category[clique[i]] == 1)
		  {p2 = p2 + 1;}
		  //else if(strcmp(G->_category[clique[i]], C3) == 0)
		  else if(G->_category[clique[i]] == 2)
		  {p3 = p3 + 1;}
		  else
		  {fprintf(stderr, "cannot find catagory for %s %s\n", G->_category[clique[i]], G->_label[clique[i]]);}
	  }
	  //printf("lb1 %d lb2 %d lb3 %d p1 %d p2 %d p3 %d\n", lb1, lb2, lb3, p1, p2, p3);
	  if( (p1 < lb1) | (p2 < lb2) | (p3 < lb3) )
	  {return;}
  }*/
  for (i = 0; i < len-1; i++)
	{//printf("G->_label[clique[i]] %s\n", G->_label[clique[i]]);
		fprintf(fp, "%s\t", G->_label[clique[i]]);
	}
  fprintf(fp, "%s\n", G->_label[clique[i]]);
  return;
}


/* ------------------------------------------------------------- *
 * Function: clique_profile_out()                                *
 * ------------------------------------------------------------- */
void clique_profile_out(FILE *fp, u64 *nclique, Graph *G)
{
  unsigned int n = G->_num_vertices;
  u64 sum=0;
  int max=0;
  int i;
  fprintf(fp, "Size\tNumber\n");
  for (i = LB; i <= n; i++) {
	if (nclique[i]) {
	  fprintf(fp, "%d\t%lu\n", i, nclique[i]);
	  sum += nclique[i];
	  max = i;
	}
  }
  fprintf(fp, "\n");
  fprintf(fp, "No. of vertices : %d\n", n);
  fprintf(fp, "No. of edges    : %d\n", G->_num_edges);
  fprintf(fp, "No. of cliques  : %lu\n", sum);
  fprintf(fp, "Max clique size : %d\n", max);
}





/* ------------------------------------------------------------- *
 * Function: clique_find_v1()                                    *
 *   Bron-Kerbosch version 1 : numerical order                   *
 *   Recursive function to find cliques                          *
 * ------------------------------------------------------------- */
void clique_find_v1(FILE *fp, u64 *nclique, Graph *G, \
		vid_t *clique, vid_t *old, int lc, int ne, int ce)
{
  vid_t new[ce];
  int new_ne, new_ce;
  vid_t u;
  int i, j;

  while (ne < ce) 
  {

#ifdef DEBUG
  for (i = 0; i < ne; i++) printf(" %s", G->_label[old[i]]);
  printf("\t|");
  for (i = 0; i < lc; i++) printf(" %s", G->_label[clique[i]]);
  printf("\t|");
  for (i = ne; i < ce; i++) printf(" %s", G->_label[old[i]]);
  printf("\n");
#endif

	u = old[ne];

	/* Set new cand and not */
	memset(new, -1, ce*sizeof(vid_t));
    new_ne = 0;
	for (j = 0; j < ne; j++)
	  if (edge_exists(G, u, old[j])) new[new_ne++] = old[j];
	new_ce = new_ne;
	for (j = ne+1; j < ce; j++)
	  if (edge_exists(G, u, old[j])) new[new_ce++] = old[j];
	
	/* Output clique or extend */
	clique[lc] = u;
	if (new_ce == 0 && lc+1 >= LB) {
	  nclique[lc+1]++;
	  //fprint("hahahah LB %d\n", LB);
	  if (PRINT) clique_out(fp, G, clique, lc+1);
	}
	else { 
	  if (new_ne < new_ce)
	    clique_find_v1(fp, nclique, G, clique, new, lc+1, new_ne, new_ce);
	}

	
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


/* ------------------------------------------------------------- *
 * Function: clique_find_v2()                                    *
 *   Bron-Kerbosch version 2                                     *
 *   Recursive function to find cliques                          *
 * ------------------------------------------------------------- */
void clique_find_v2(FILE *fp, u64 *nclique, Graph *G, \
		vid_t *clique, vid_t *old, int lc, int ne, int ce)
{
  vid_t new[ce];
  int new_ne, new_ce;
  vid_t fixp=0, p, u;
  int s=0, pos=0, nod, minnod, count;
  int i, j, k;

  if (lc + (ce - ne) < LB) return;

#ifdef DEBUG
  for (i = 0; i < ne; i++) printf(" %s", G->_label[old[i]]);
  printf("\t|");
  for (i = 0; i < lc; i++) printf(" %s", G->_label[clique[i]]);
  printf("\t|");
  for (i = ne; i < ce; i++) printf(" %s", G->_label[old[i]]);
  printf("\n");
#endif

  /* Choose a vertex, fixp, in old (both not and cand) that
	 has lowest number of non-adjacent vertices in old cand */
  minnod = ce + 1;
  nod = 0;
  for (i = 0; i < ce; i++) {
	count = 0;
	p = old[i];
	for (j = ne; j < ce; j++) {
	  if (!edge_exists(G, p, old[j])) {
		count++;
		pos = j;
	  }
	}
	if (count < minnod) {
	  fixp = p;
	  minnod = count;
	  if (i < ne) { s = pos; }    // if p in not
	  else { s = i; nod = 1; }    // if p in cand
	}
  }
  
  /* Recursively extend clique */
  for (k = minnod+nod; k > 0; k--) {

	/* Swap this candidate to be the next one */
	p = old[s];
	old[s] = old[ne];
	old[ne] = p;

	u = old[ne];

	/* Set new cand and not */
	memset(new, -1, ce*sizeof(vid_t));
    new_ne = 0;
	for (j = 0; j < ne; j++)
	  if (edge_exists(G, u, old[j])) new[new_ne++] = old[j];
	new_ce = new_ne;
	for (j = ne+1; j < ce; j++)
	  if (edge_exists(G, u, old[j])) new[new_ce++] = old[j];
	
	/* Output clique or extend */
	clique[lc] = u;
	if (lc+1 <= UB) {
	  if (new_ce == 0 && lc+1 >= LB) {
		//printf("hahahah LB %d\n", LB);
	    nclique[lc+1]++;
	    if (PRINT) clique_out(fp, G, clique, lc+1);
	  }
	  else if (new_ne < new_ce) {
	    clique_find_v2(fp, nclique, G, clique, new, lc+1, new_ne, new_ce);
	  }
	}
	
	/* Move u to not */
	ne++;

	/* Bound condition: Stop if fixp is a neighbor of all candidates */ 
    if (k > 1) {
	  for (s = ne; s < ce; s++) {
	    if (!edge_exists(G, fixp, old[s])) break;
	  }
	  if (s == ce) return;
	}

  }

  return;
}


/* ------------------------------------------------------------- *
 * Function: maxclique_find()                                    *
 *   Bron-Kerbosch version 2                                     *
 *   Recursive function to find one of the maximum cliques       *
 * ------------------------------------------------------------- */
void maxclique_find(vid_t *maxclique, int *maxclique_size, Graph *G, \
		vid_t *clique, vid_t *old, int lc, int ne, int ce)
{
  vid_t new[ce];
  int new_ne, new_ce;
  vid_t fixp=0, p, u;
  int s=0, pos=0, nod, minnod, count;
  int i, j, k;

#ifdef DEBUG
  for (i = 0; i < ne; i++) printf(" %d", old[i]);
  printf("\t|");
  for (i = 0; i < lc; i++) printf(" %d", clique[i]);
  printf("\t|");
  for (i = ne; i < ce; i++) printf(" %d", old[i]);
  printf("\n");
#endif

  /* Choose a vertex, fixp, in old (both not and cand) that
	 has lowest number of non-adjacent vertices in old cand */
  minnod = ce + 1;
  nod = 0;
  for (i = 0; i < ce; i++) {
	count = 0;
	p = old[i];
	for (j = ne; j < ce; j++) {
	  if (!edge_exists(G, p, old[j])) {
		count++;
		pos = j;
	  }
	}
	if (count < minnod) {
	  fixp = p;
	  minnod = count;
	  if (i < ne) { s = pos; }    // if p in not
	  else { s = i; nod = 1; }    // if p in cand
	}
  }
  
  /* Recursively extend clique */
  for (k = minnod+nod; k > 0; k--) {

	/* Swap this candidate to be the next one */
	p = old[s];
	old[s] = old[ne];
	old[ne] = p;

	u = old[ne];

	/* Set new cand and not */
	memset(new, -1, ce*sizeof(vid_t));
    new_ne = 0;
	for (j = 0; j < ne; j++)
	  if (edge_exists(G, u, old[j])) new[new_ne++] = old[j];
	new_ce = new_ne;
	for (j = ne+1; j < ce; j++)
	  if (edge_exists(G, u, old[j])) new[new_ce++] = old[j];
	
	/* Record clique or extend */
	clique[lc] = u;
	if (new_ce == 0 && lc+1 >= *maxclique_size) {
	  *maxclique_size = lc + 1;
	  memcpy(maxclique, clique, (*maxclique_size)*sizeof(vid_t));
	  if (PRINT) {
		printf("max size %d\n", lc+1);
		clique_out(stdout, G, clique, lc+1);
	  }
	}
	else if (new_ce-new_ne+lc+1 >= *maxclique_size) {
	  maxclique_find(maxclique, maxclique_size, G, clique, new, lc+1, new_ne, new_ce);
	}
	
	/* Move u to not */
	ne++;

	/* Bound condition: Stop if fixp is a neighbor of all candidates */ 
    if (k > 1) {
	  for (s = ne; s < ce; s++) {
	    if (!edge_exists(G, fixp, old[s])) break;
	  }
	  if (s == ce) return;
	}

  }

  return;
}


/* ------------------------------------------------------------- *
 * Function: clique_enumerate()                                  
 *   For mpiclique version 1, 2
 * ------------------------------------------------------------- */

void clique_enumerate(FILE *fp, u64 *nclique, Graph *G, vid_t *cand, int lcand)
{
  unsigned int n = num_vertices(G);
  vid_t clique[n];
  vid_t vertices[n];
  int ne, ce;
  vid_t u, i, j;
#ifdef DEBUG 
  double utime;
#endif
  
  memset(clique, -1, n*sizeof(vid_t));

  for (i = 0; i < lcand; i++) {

#ifdef DEBUG
    utime = get_cur_time();
#endif
	
	u = cand[i];

	/* Prepare cand and not array */
	ne = 0;
	for (j = 0; j < u; j++)
	  if (edge_exists(G, j, u)) vertices[ne++] = j;
	ce = ne;
	for (j = u+1; j < n; j++)
	  if (edge_exists(G, u, j)) vertices[ce++] = j;

	/* Recursively find cliques containing u if enough candidates */
	if (ce - ne >= LB - 1) {
	  clique[0] = u;
	  if (VERSION == 1)
        clique_find_v1(fp, nclique, G, clique, vertices, 1, ne, ce);
	  else if (VERSION == 2)
        clique_find_v2(fp, nclique, G, clique, vertices, 1, ne, ce);
    }

#ifdef DEBUG
	utime = get_cur_time() - utime;
	printf("task %4d : %d subtasks, %f seconds\n", u, ce-ne, utime);
#endif
  }
  
  return;
}

/* ------------------------------------------------------------- *
 * Function: clique_find_v4()                                    *
 *   Bron-Kerbosch version 4 : numerical order                   *
 *   Recursive function to find k-partite cliques                *
 * ------------------------------------------------------------- */
 //vid_t *clique(R): current clique
 //vid_t *old(P): X cascade P
 //ne: size of X
 //ce: size of X + P
//nclique: used for hist graph for clique length
//lc: clique length - 1
/*void clique_find_v4(FILE *fp, u64 *nclique, Graph *G, \
		vid_t *clique, vid_t *old, int * csizes[], int lc, int ne, int ce, int psizes[])*/
void clique_find_v4(FILE *fp, u64 *nclique, Graph *G, \
		vid_t *clique, vid_t *old, int lc, int ne, int ce, int * csizes, int * psizes)
{
  vid_t new[ce];
  int new_ne, new_ce;
  vid_t u, pivot, tmpu;
  int i, j, pnei;//neibor of pivot vertex should be in P
  //vid_t newvertex[Spart][ce];
  int new_psizes[G->Pnum];
  int upid, jpid;
  int parclique = 0;
  //printf("lc ne ce %d %d %d\n", lc, ne, ce);
  for(i = 0; i < G->Pnum; i++)
  {
	//printf("i, csizes[i], psizes[i] %d %d %d\n", i, csizes[i], psizes[i]);
	  if(csizes[i] == 0 && psizes[i] == 0)
	  {
	//	printf("i %d\n", i);
		return;}
  }
  pivot = old[ne];
  pnei = ce -1;
  while (ne < ce) 
  {
	u = old[ne];
	upid = G->_category[u];
	if (u != pivot)
	{
		if(edge_exists(G, u, pivot) || (G->_category[u] == G->_category[pivot]))
		{
			//ne++;
			tmpu = u;
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
	//printf("ne u category label %d %d %s %s\n", ne, u, G->_categoryname[G->_category[u]], G->_label[u]);
	/* Set new cand and not */
	memset(new, -1, ce*sizeof(vid_t));
	memset(new_psizes, 0, G->Pnum * sizeof(int));
    new_ne = 0;
	for (j = 0; j < ne; j++)
	{
	  if (edge_exists(G, u, old[j]) || (G->_category[old[j]] == G->_category[u])) 
	  {
		  jpid = G->_category[old[j]];
		  //newvertex[jpid][new_psizes[jpid]] = old[j];
		  //new_psizes[jpid]++;
		  new[new_ne++] = old[j];
	  }
	}
	new_ce = new_ne;
	for (j = ne+1; j < ce; j++)
	{
	  if (edge_exists(G, u, old[j]) || (G->_category[old[j]] == G->_category[u]))
	  {
		  jpid = G->_category[old[j]];
		  new_psizes[jpid]++;
		  new[new_ce++] = old[j];
	  }
	}
	
	/* Output clique or extend */
	upid = G->_category[u];
	clique[lc] = u;
	csizes[upid]++;
	//printf("new_ce %d\n", new_ce);
	if(new_ce == 0) 
	{
	  parclique = 0;
	  //printf("????????\n");
	  for(i = 0; i < G->Pnum; i++)
	  {
		  //printf("i %d %d\n", i, csizes[i]);
		  //if(csizes[i] == 0)
		  //printf("csizes[i] G->lbs[i] %d %d\n", csizes[i], G->lbs[i]);
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
	    clique_find_v4(fp, nclique, G, clique, new, lc+1, new_ne, new_ce, csizes, new_psizes);
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