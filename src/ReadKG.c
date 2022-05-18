#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include "graph.h"
Graph * ReadKG(FILE *fp)
{
    unsigned int N, E, C;
    char * line = NULL;
    char * pch;
    size_t len;
    ssize_t read;
    char * word1;
    char * word2;
    ENTRY item;
    ENTRY *found_item;
    int u, v;
    int k=0, edges=0, r;
    Graph *G;
    int *id, *ids;
    
    while ((read = getline(&line, &len, fp)) != -1) {
        pch = NULL;
        pch = strtok(line, " ");
        if(*pch == 'c')
        {continue;}
        else if(*pch == 'p')
        {
            char * edges = "edges";
            pch = strtok (NULL, " ");
            if(strcmp(pch, edges) != 0)
            {
                fprintf(stderr, "Bad file format : p edges N E C incorrect\n");
	            exit(-1);
            }
            pch = strtok (NULL, " ");
            N = atoi(pch);
            pch = strtok (NULL, " ");
            E = atoi(pch);
            pch = strtok (NULL, " ");
            C = atoi(pch);
            printf("N %d E %d C %d\n", N, E, C);
            G = graph_make(N);
            (void) hcreate(N);
            ids = (int *) malloc(N * sizeof(int));
            G->Pnum = C;
        }
        else if(*pch == 'e')
        {
            pch = strtok (NULL, " ");
            word1 = pch;
            pch = strtok (NULL, " ");
            word2 = pch;
            item.key = word1;
	        if ((found_item = hsearch(item, FIND)) != NULL) {
	            id = (int *) (found_item->data);
	            u = *id;
	        }
	        else {
	            u = k;
	            G->_label[k++] = strdup(word1);
	            item.key = G->_label[u];
	            ids[u] = u;
	            item.data = (void *) (ids+u);
	            (void) hsearch(item, ENTER);
	        }
            item.key = word2;
	        if ((found_item = hsearch(item, FIND)) != NULL) {
	            id = (int *) (found_item->data);
	            v = *id;
	        }
	        else {
	            v = k;
	            G->_label[k++] = strdup(word2);
	            item.key = G->_label[v];
	            ids[v] = v;
	            item.data = (void *) (ids+v);
	            (void) hsearch(item, ENTER);
	        }
            if (k > N) {
	            fprintf(stderr, "Bad file format : too many labels\n");
	            exit(-1);
	        }
	        add_edge(G, u, v);
	        edges++;
        }
        else if(*pch == 'n')
        {
            G->psizes = (int *)malloc(G->Pnum * sizeof(int));
            G->_category = (int *)malloc(G->Pnum * sizeof(int));
            G->_categoryname = (char **)malloc(G->Pnum * sizeof(char *));
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

}

int main()
{
    FILE *fp;
    fp = fopen("test.kg", "r");
    ReadKG(fp);
    return 0;
}