// You can find instructions for this file at http://www.treeview.net

//Environment variables are usually set at the top of this file.
USETEXTLINKS = 1
STARTALLOPEN = 1
USEFRAMES = 0
USEICONS = 0
WRAPTEXT = 1
PRESERVESTATE = 1
HIGHLIGHT = 1


foldersTree = gFld("<b>Tree Options</b>", "demoFramelessHili.html")
    aux2 = insFld(foldersTree, gFld("United States", "demoFramelessHili.html?pic=%22beenthere_unitedstates%2Egif%22"))
 			insDoc(aux2, gLnk("S", "Boston", "demoFramelessHili.html?pic=%22beenthere_boston%2Ejpg%22"))
 			insDoc(aux2, gLnk("S", "Tiny pic of New York City", "demoFramelessHili.html?pic=%22beenthere_newyork%2Ejpg%22"))
 			insDoc(aux2, gLnk("S", "Washington", "demoFramelessHili.html?pic=%22beenthere_washington%2Ejpg%22"))
    aux2 = insFld(foldersTree, gFld("Europe", "javascript:undefined"))
      insDoc(aux2, gLnk("S", "London", "demoFramelessHili.html?pic=%22beenthere_london%2Ejpg%22"))
      insDoc(aux2, gLnk("S", "Lisbon", "demoFramelessHili.html?pic=%22beenthere_lisbon%2Ejpg%22"))

//Set this string if Treeview and other configuration files may also be loaded in the same session
foldersTree.treeID = "t2" 
 