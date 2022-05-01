/* Driver file for Clique Enumerator by BK
 *
 * Author: Yun Zhang, yzhang@cs.utk.edu
 * Version: 2.1.a
 * Last modified: 08-18-2008
 *
 * Changes: Add the upper bound of clique size to the BK 
 * 
 * Copyright 2005-2006
 * Department of Computer Science, University of Tennessee, Knoxville
 *
 */

#include <string.h>
#include "bit.h"
#include "graph.h"
#include "utility.h"
#include "bk.h"
//#include "PartiteSize.h"


/* Global variables */
extern int LB,UB;  // lower bound and upper bound of clique size
extern int VERSION;
extern int PRINT;
extern int FILTER;
extern int PART;
extern int lb1, lb2, lb3;
extern int Spart;
FILE *fp;
char *outfn, infn[100];
extern char * FilterName;


void print_options(void)
{
  fprintf(stderr, "\n Options: \n");
  fprintf(stderr, "  -p             print out clique list \n");
  fprintf(stderr, "                 note: no print out if not specify -p\n");
  fprintf(stderr, "                       print to stdout if not specify -o\n");
  fprintf(stderr, "  -o <filename>  filename to store cliques if choose to print out\n");
  fprintf(stderr, "  -l <value>     least number of vertices in a clique <default = 3>\n");
  fprintf(stderr, "  -u <value>     most number of vertices in a clique <default = graph size>\n");
  fprintf(stderr, "  -v [1|2|3]     algorithm version <default = 2>\n");
  fprintf(stderr, "                 1 - bron kerbosch version 1 (numerical order)\n");
  fprintf(stderr, "                 2 - bron kerbosch version 2 (improved version)\n");
  fprintf(stderr, "                 3 - modified bron kerbosch to find maximum clique\n");
  fprintf(stderr, "\n");
}

void argument_parse(int argc, char **argv)
{
  int i;
  
  if (argc < 2) {
    fprintf(stderr, "Usage: %s Graph <options>\n", argv[0]);
    print_options();
	exit(1);
  }
  
  LB = 3; UB = -1;
  VERSION = 2;
  PRINT = 0;
  FILTER = 0;
  PART = 1;
  outfn = NULL;
  lb1 = lb2 = lb3 = 5;
  
  
  for (i = 2; i < argc; i++) {
	if (!strcmp(argv[i], "-o")) {
	  outfn = strdup(argv[++i]);
	}
	if (!strcmp(argv[i], "-l")) {
	  LB = atoi(argv[++i]);
	}
	if (!strcmp(argv[i], "-u")) {
	  UB = atoi(argv[++i]);
	}
	if (!strcmp(argv[i], "-v")) {
	  VERSION = atoi(argv[++i]);
	}
	if (!strcmp(argv[i], "-p")) {
	  PRINT = 1;
	}
  if (!strcmp(argv[i], "-f")) {
	  FILTER = 1;
    FilterName = argv[i + 1];
    //readpartite(partitefile);
    lb1 = atoi(argv[i + 2]);
    lb2 = atoi(argv[i + 3]);
    lb3 = atoi(argv[i + 4]);
	}
  }

  strcpy(infn, argv[1]);
  if ((fp = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Can't open file %s\n", argv[1]);
    exit(-1);
  }

  return;
}


void maximal_clique(Graph *G)
{
  FILE *fp1=stdout, *fp2;
  char fname[100];
  double utime;
  unsigned int n = num_vertices(G);
  vid_t clique[n];
  vid_t vertices[n];
  u64 nclique[n+1];
  int i, pid;

  if (outfn != NULL) {
    sprintf(fname, "%s.clique", outfn);
    fp1 = fopen(fname, "w");
  }
  sprintf(fname, "%s.profile", infn);
  fp2 = fopen(fname, "w");

  utime = get_cur_time();
  memset(nclique, 0, (n+1)*sizeof(u64));
  memset(clique, -1, n*sizeof(vid_t));
  for (i = 0; i < n; i++) vertices[i] = i;
  if (VERSION == 1)
	{clique_find_v1(fp1, nclique, G, clique, vertices, 0, 0, n);}
  else if (VERSION == 2)
	{clique_find_v2(fp1, nclique, G, clique, vertices, 0, 0, n);}
  else if(VERSION == 4)
  {
    int psizes[Spart];//each partite has how much nodes
    int csizes[Spart];//each partite has how much nodes
    memset(psizes, 0, Spart*sizeof(int));
    memset(csizes, 0, Spart*sizeof(int));
    for (i = 0; i < n; i++)
    {
      pid = G->_category[i];
      psizes[pid]++;
    }
    clique_find_v4(fp1, nclique, G, clique, vertices, 0, 0, n, csizes, psizes);
  }
  utime = get_cur_time() - utime;


  clique_profile_out(fp2, nclique, G);
  fprintf(fp2, "Time (seconds)  : %.6f\n", utime);

  if (outfn != NULL) free(outfn);
  fclose(fp1);
  fclose(fp2);

}

/*void maximal_clique_v4(Graph *G)
{
  FILE *fp1=stdout, *fp2;
  char fname[100];
  double utime;
  unsigned int n = num_vertices(G);
  //vid_t clique[Spart][n];
  vid_t clique[n];
  vid_t vertices[Spart][n];
  vid_t X[Spart][n];
  int psizes[Spart];//each partite has how much nodes
  int csizes[Spart];//each partite has how much nodes
  int xsizes[Spart];
  int pid;//partite ID
  u64 nclique[n+1];
  int i;

  if (outfn != NULL) {
    sprintf(fname, "%s.clique", outfn);
    fp1 = fopen(fname, "w");
  }
  sprintf(fname, "%s.profile", infn);
  fp2 = fopen(fname, "w");

  utime = get_cur_time();
  memset(nclique, 0, (n+1)*sizeof(u64));
  //memset(clique, -1, (Spart * n)*sizeof(vid_t));
  memset(clique, -1, n*sizeof(vid_t));
  //memset(vertices, -1, (Spart * n)*sizeof(vid_t));
  memset(X, -1, (Spart * n)*sizeof(vid_t));
  memset(psizes, 0, Spart*sizeof(int));
  memset(csizes, 0, Spart*sizeof(int));
  memset(xsizes, 0, Spart*sizeof(int));

  for (i = 0; i < n; i++)
  {
    pid = G->_category[i];
    vertices[pid][sizes[pid]] = i;
    G->_intraid[i] = psizes[pid];
    psizes[pid]++;
  }
  clique_find_v4(fp1, nclique, G, clique, vertices, csizes, 0, n, psizes);
  


}*/

void maximum_clique(Graph *G)
{
  FILE *fp=stdout;
  unsigned int n = num_vertices(G);
  vid_t clique[n];
  vid_t vertices[n];
  vid_t maxclique[n];
  int maxclique_size=0;
  vid_t i;

  if (outfn != NULL) fp = fopen(outfn, "w");
  
  memset(clique, -1, n*sizeof(vid_t));
  for (i = 0; i < n; i++) vertices[i] = i;
  maxclique_find(maxclique, &maxclique_size, G, clique, vertices, 0, 0, n);

  if (PRINT) clique_out(fp, G, maxclique, maxclique_size);
  fprintf(fp, "Maximum Clique Size : %d\n", maxclique_size);

  if (outfn != NULL) free(outfn);
  fclose(fp);
  return;
}


int main(int argc, char  **argv)
{
  Graph *G;

  argument_parse(argc, argv);
  
  G = graph_edgelist_in(fp, FILTER, PART);
  fclose(fp);
  if (UB == -1) UB = num_vertices(G);

  if (VERSION == 3) maximum_clique(G);
  else maximal_clique(G);

  graph_free(G);

  exit(0);
}

