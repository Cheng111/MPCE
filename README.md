# MPCE
maximal partiteclique enumaration

##Usage
Usage: ./mpce Graph <options>
Options:
-p				print out clique list
				note: no print out if not specify -p
-o <filename>	filename to store cliques if choose to print out
				print to stdout if not specify -o
-l <value>		least number of vertices in a clique <default = 3>
-u <value>		most number of vertices in a clique <default = graph size>
-v [1|2|3|4]	algorithm version <default = 2>
					1 - bron kerbosch version 1 (numerical order)
					2 - bron kerbosch version 2 (improved version)
					3 - modified bron kerbosch to find maximum clique
					4 - maximal partite cliques enumration (BK with piivot)
					5 - maximal partite cliques enumration (add intra-partite edges)
--fconf			configure file of maximal partite cliques enumration

## example usage
- Maximal k-partite clique enumration
    - ./mpce test/test_K222.txt -v 4 --fconf kpartite.conf -p
- example comfigure file: kpartite.conf. The configure file store the lower bound of the vertices in each kpartite clique partite

## Input file format requirement
