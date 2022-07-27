# MPCE
maximal partiteclique enumaration

## example usage
- Maximal k-partite clique enumration
    - ./mpce test/test_K222.txt -v 4 --fconf kpartite.conf -p
- example comfigure file: kpartite.conf. The configure file store the lower bound of the vertices in each kpartite clique partite

## Input file format requirement
- The vertices and color should not have same name. For instance, if vertices' names are 1 ~ n. The color name could be "red", " blue" or n + 1, n + 2; however the color name could not be any number between 1 ~ n. 
