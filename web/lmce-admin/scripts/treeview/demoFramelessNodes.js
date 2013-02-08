// You can find instructions for this file at http://www.treeview.net

//Environment variables are usually set at the top of this file.
USETEXTLINKS = 1
STARTALLOPEN = 0
USEFRAMES = 0
USEICONS = 0
WRAPTEXT = 1
PRESERVESTATE = 1


foldersTree = gFld("<b>Tree Options</b>", "demoFrameless.html")
  aux1 = insFld(foldersTree, gFld("Expand for example with pics and flags", "javascript:undefined"))
    aux2 = insFld(aux1, gFld("United States", "demoFrameless.html?pic=%22beenthere_unitedstates%2Egif%22"))
 			insDoc(aux2, gLnk("S", "Boston", "demoFrameless.html?pic=%22beenthere_boston%2Ejpg%22"))
 			insDoc(aux2, gLnk("S", "Tiny pic of New York City", "demoFrameless.html?pic=%22beenthere_newyork%2Ejpg%22"))
 			insDoc(aux2, gLnk("S", "Washington", "demoFrameless.html?pic=%22beenthere_washington%2Ejpg%22"))
    aux2 = insFld(aux1, gFld("Europe", "demoFrameless.html?pic=%22beenthere_europe%2Egif%22"))
      insDoc(aux2, gLnk("S", "London", "demoFrameless.html?pic=%22beenthere_london%2Ejpg%22"))
      insDoc(aux2, gLnk("S", "Lisbon", "demoFrameless.html?pic=%22beenthere_lisbon%2Ejpg%22"))
  aux1 = insFld(foldersTree, gFld("Types of node", "javascript:undefined"))
    aux2 = insFld(aux1, gFld("Expandable with link", "demoFrameless.html?pic=%22beenthere_europe%2Egif%22"))
      insDoc(aux2, gLnk("S", "London", "demoFrameless.html?pic=%22beenthere_london%2Ejpg%22"))
    aux2 = insFld(aux1, gFld("Expandable without link", "javascript:undefined"))
 			insDoc(aux2, gLnk("S", "NYC", "demoFrameless.html?pic=%22beenthere_newyork%2Ejpg%22"))
    insDoc(aux1, gLnk("B", "Opens in new window", "http://www.treeview.net/treemenu/demopics/beenthere_pisa.jpg"))
 