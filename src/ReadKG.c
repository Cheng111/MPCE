#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include "graph.h"

Graph * ReadKG(FILE *fp)
{
    unsigned int N = 0, E = 0, C = 0;
    char * line = NULL;
    char * pch;
    size_t len;
    ssize_t read;
    char * word1;
    char * word2;
    char * color;
    char * delim = " \t";
    ENTRY item;
    ENTRY *found_item;
    ENTRY citem;
    ENTRY *found_citem;
    struct hsearch_data *htab;
    struct hsearch_data *htabc;
    int u, v = -1, cid = -1;
    int k=0, edges=0, cid_use = 0;
    Graph * G = NULL;
    int *id;
    int *cids = NULL;
    int *ids = NULL;
    
    while ((read = getline(&line, &len, fp)) != -1) {
        while(line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\x0d'){
            line[strlen(line) - 1] = 0;
            if(strlen(line) == 0)
            {break;}
        }
        if(strlen(line) == 0)
        {continue;}
        pch = NULL;
        pch = strtok(line, delim);
        if(*pch == 'c')
        {continue;}
        else if(*pch == 'p')
        {
            char * edges = "edges";
            pch = strtok (NULL, delim);
            if(strcmp(pch, edges) != 0)
            {
                fprintf(stderr, "Bad file format : p edges N E C incorrect\n");
	            exit(-1);
            }
            pch = strtok (NULL, delim);
            N = atoi(pch);
            pch = strtok (NULL, delim);
            E = atoi(pch);
            pch = strtok (NULL, delim);
            C = atoi(pch);
            //printf("N %d E %d C %d\n", N, E, C);
            G = graph_make(N);
            //(void) hcreate(N + C + 100);
            htab=calloc(1,sizeof(struct hsearch_data));
            hcreate_r(N + 100,htab);
            htabc=calloc(1,sizeof(struct hsearch_data));
            hcreate_r(C + 100,htabc);

            ids = (int *) malloc(N * sizeof(int));
            cids = (int *) malloc(C * sizeof(int));
            G->Pnum = C;
            //(void) hcreate(C);
            G->psizes = (int *)malloc(G->Pnum * sizeof(int));
            G->_category = (int *)malloc(N * sizeof(int));
            G->_categoryname = (char **)malloc(G->Pnum * sizeof(char *));
        }
        else if(*pch == 'e')
        {
            pch = strtok (NULL, delim);
            word1 = pch;
            pch = strtok (NULL, delim);
            word2 = pch;
            item.key = word1;
	        //if ((found_item = hsearch(item, FIND)) != NULL) {
            if (hsearch_r(item, FIND, &found_item, htab) != 0) {
	            id = (int *) (found_item->data);
	            u = *id;
	        }
	        else {
	            u = k;
	            G->_label[k++] = strdup(word1);
                //printf("G->_label[k++] u %s %d %d\n", word1, u, k);
	            item.key = G->_label[u];
	            ids[u] = u;
	            item.data = (void *) (ids+u);
	            //(void) hsearch(item, ENTER);
                hsearch_r(item, ENTER, &found_item, htab);
	        }
            item.key = word2;
	        //if ((found_item = hsearch(item, FIND)) != NULL) {
            if (hsearch_r(item, FIND, &found_item, htab) != 0) {
	            id = (int *) (found_item->data);
	            v = *id;
	        }
	        else {
	            v = k;
	            G->_label[k++] = strdup(word2);
                //printf("G->_label[k++] v %s %d %d\n", word2, v, k);
	            item.key = G->_label[v];
	            ids[v] = v;
	            item.data = (void *) (ids+v);
	            //(void) hsearch(item, ENTER);
                hsearch_r(item, ENTER, &found_item, htab);
	        }
            if (k > N) {
	            fprintf(stderr, "Bad file format : too many labels %d %d\n", k, N);
	            exit(-1);
	        }
	        add_edge(G, u, v);
	        edges++;
        }
        else if(*pch == 'n')
        {
            pch = strtok (NULL, delim);
            word1 = pch;
            pch = strtok (NULL, delim);
            color = pch;
            //printf("word1 %s color %s\n", word1, color);
            item.key = word1;
	        //if ((found_item = hsearch(item, FIND)) != NULL){
            if (hsearch_r(item, FIND, &found_item, htab) != 0) {
	            id = (int *) (found_item->data);
	            v = *id;
            }
            citem.key = color;
            //if ((found_citem = hsearch(citem, FIND)) != NULL){
            if (hsearch_r(citem, FIND, &found_citem, htabc) != 0) {
	            id = (int *) (found_citem->data);
	            cid = *id;
            }
            else{
                G->_categoryname[cid_use] = strdup(color);
                cid = cid_use;
                cid_use++;
                cids[cid] = cid;
                citem.key = G->_categoryname[cid];
                //printf("G->_categoryname[cid] %s\n", G->_categoryname[cid]);
                citem.data = (void *) (cids + cid);
                //hsearch(citem, ENTER);
                hsearch_r(citem, ENTER, &found_citem, htabc);
            }
            //printf("%s %d %s %d\n",word1, v, color, cid);
            G->_category[v] = cid;
        }
        else{
            fprintf(stderr, "unknown command %c\n", *pch);
        }
    }
    if (edges != E) { 
	    fprintf(stderr, "edgelist_in : # of edges incorrect\n");
	    fprintf(stderr, "edgelist_in : %d vertices, %d edges\n", k, edges);
    }
    if (k != N) {
	    fprintf(stderr, "edgelist_in : # of vertices incorrect\n");
	    fprintf(stderr, "edgelist_in : %d vertices, %d edges\n", k, edges);
	    G->_num_vertices = k;
	    G->_num_active_vertices = k;
    }
    hdestroy();
    return G;
}

void printGraph(Graph * G, FILE * fout)
{
    int i;
    //unsigned int v;
    //unsigned int * tmp;
    AdjMatrix_out(fout, G);
    for(i = 0; i < G->_num_vertices; i++)
    {
        printf("node %d %s color %d %s\n", i, G->_label[i], G->_category[i], G->_categoryname[G->_category[i]]);
    }
}

void GetConfig(char * confile, Graph * G)
{
    char * line = NULL;
    size_t len;
    char * pch;
    ssize_t read;
    char * delim = " \t";
    int i;
    const char * tlb = "lb";
    FILE * fconf;
    if ((fconf = fopen(confile, "r")) == NULL) 
    {
        fprintf(stderr, "Can't open file %s\n", confile);
        exit(-1);
    }
    int lb;
    //printf("G->Pnum %d\n", G->Pnum);
    G->lbs = (int *) malloc(G->Pnum * sizeof(int));
    while ((read = getline(&line, &len, fconf)) != -1) {
        while(line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\x0d'){
            line[strlen(line) - 1] = 0;
            if(strlen(line) == 0)
            {break;}
        }
        if(strlen(line) == 0)
        {continue;}
        pch = NULL;
        pch = strtok(line, delim);
        //printf("pch %s\n", pch);
        if(strcmp(pch, tlb) == 0){
            pch = strtok (NULL, delim);
            lb = atoi(pch);
            //printf("lb %d\n", lb);
            for(i = 0; i < G->Pnum; i++)
            {G->lbs[i] = lb;}
        }
    }
    fclose(fconf);
}

//Use for debug
/*int main(int argc, char  **argv)
{
    FILE * fp = fopen(argv[1], "r");
    Graph *  G = ReadKG(fp);
    GetConfig(argv[2], G);
    int i;
    for(i = 0; i < G->Pnum; i++)
    {printf("%d ", G->lbs[i]);}
    printf("\n");
}*/