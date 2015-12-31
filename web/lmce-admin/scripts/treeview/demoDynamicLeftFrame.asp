<%@ Language=VBScript %>
<% 
option explicit 
Response.Expires=-1
%>
<!-- The Response.Expires=-1 will make sure the page is not cached.
     If the page is cached and the database changes, the new tree will not be shown -->

<!--
     (Please keep all copyright notices.)
     This page document includes the Treeview script.
     Script found at: http://www.treeview.net
     Author: Marcelino Alves Martins
-->

<%

Dim databaseDir, Conn

'Change this to a path (c:\...) if the database is not in the same dir of the 
'current file
databaseDir = Server.MapPath("demoDynamic.mdb")
Set Conn = Server.CreateObject("ADODB.Connection")
Conn.Open("DRIVER={Microsoft Access Driver (*.mdb)}; DBQ=" & databaseDir)


' This is a recursive function; it will find the children directly under a node
' and then call itself for each of those children in order to find the grand-children.
' For each entry in the DB that this function finds, it sends a snippet of 
' JavaScript to the browser with the Treeview commands necessary for the 
' construction of a node (folder or "doc")
' 
'   conn: is an ADODB.Connection opened on a database with a NodesTable table
'   parentId: is the value of the ParentID field for a record in the database
'   parentObject: is the name of the JavaScript variable used to define the parent

sub outputJavascriptForRoot(Conn)
    dim rsHits, queryString
    queryString = "SELECT NodeID, NodeName, IsFolder, ParentID, Link FROM NodesTable WHERE (ParentID=-1)"

    Set rsHits = Server.CreateObject("ADODB.Recordset")
    rsHits.Open queryString, Conn ' It should reeturn one and one only

    outputJavascriptForSubFolder Conn, rsHits("NodeID"), rsHits("NodeName"), "fSub1"
    
    response.write "foldersTree = fSub1"

end sub

sub outputJavascriptForSubFolder(Conn, folderId, nodeName, fName)
    dim rsHits, queryString, gFldStr, gLnkStr, fi, subFolders, di

    queryString = "SELECT NodeID, NodeName, IsFolder, ParentID, Link FROM NodesTable WHERE ((ParentID=" & folderId & ") AND (IsFolder=True)) ORDER BY NodeName"

    Set rsHits = Server.CreateObject("ADODB.Recordset")
    rsHits.Open queryString, Conn

    fi=1
    do while not rsHits.EOF
        outputJavascriptForSubFolder Conn, rsHits("NodeID"), rsHits("NodeName"), fName & "Sub" & fi
        rsHits.MoveNext
        fi=fi+1
    loop

    response.write fName & " = " & "gFld('" & nodeName & "', 'javascript:parent.op();')" & VbCrLf
    response.write fName & ".xID = " & folderId & VbCrLf

    ' Call the addChildren function
    response.write fName & ".addChildren(["

    ' member of the list argument to addChildren
    if not rsHits.BOF then rsHits.MoveFirst()
    fi=1
    do while not rsHits.EOF
        if fi>1 then response.write ", "
        response.write fName & "Sub" & fi
        rsHits.MoveNext
        fi=fi+1
    loop
    rsHits.close
    subFolders = fi-1 'Count how many

    queryString = "SELECT NodeID, NodeName, Link FROM NodesTable WHERE ((ParentID=" & folderId & ") AND (IsFolder=False)) ORDER BY NodeName"
    rsHits.Open queryString, Conn
    di = 1
    do while not rsHits.EOF
        if di>1 or subFolders > 0 then response.write ", "
        response.write "['" & rsHits("NodeName") & "', '" & rsHits("Link") &"']"
        rsHits.MoveNext
        di = di + 1
    loop

    response.write "])" & VbCrLf 'Close addChildren function

    ' xID's for docs
    if not rsHits.BOF then rsHits.MoveFirst()
    di = subFolders
    do while not rsHits.EOF
        response.write fName & ".children[" & di & "].xID = " & rsHits("NodeID") & VbCrLf
        rsHits.MoveNext
        di = di+1
    loop

    rsHits.close

end sub

%>

<html>
<head>

<title>Tree from database</title>

<style>
   BODY {background-color: white}
   TD {font-size: 10pt; 
       font-family: verdana,helvetica; 
	   text-decoration: none;
	   white-space:nowrap;}
   A  {text-decoration: none;
       color: black}
</style>

<!-- As in a client-side built tree, all the tree infrastructure is put in place
     within the HEAD block, but the actual tree rendering is trigered within the
     BODY -->

<!-- Code for browser detection -->
<script src="ua.js"></script>

<!-- Infrastructure code for the tree -->
<script src="ftiens4.js"></script>

<!-- Execution of the code that actually builds the specific tree.
     The variable foldersTree creates its structure with calls to
	 gFld, insFld, and insDoc -->
<script>
USETEXTLINKS = 1
STARTALLOPEN = 0
PRESERVESTATE = 1
ICONPATH = '' 
HIGHLIGHT = 1
<% 
outputJavascriptForRoot Conn
%>


// Load a page as if a node on the tree was clicked (synchronize frames)
// (Highlights selection if highlight is available.)
function loadSynchPage(xID) 
{
	var folderObj;
	docObj = parent.treeframe.findObj(xID);
	docObj.forceOpeningOfAncestorFolders();
	parent.treeframe.clickOnLink(xID,docObj.link,'basefrm'); 

    //Scroll the tree window to show the selected node
    //Other code in these functions needs to be changed to work with
    //frameless pages, but this code should, I think, simply be removed
    if (typeof parent.treeframe.document.body != "undefined") //scroll doesn work with NS4, for example
        parent.treeframe.document.body.scrollTop=docObj.navObj.offsetTop
} 
</script>
</head>

<body topmargin=16 marginheight=16>

<!-- By removing the follwoing code you are violating your user agreement.
     Corporate users or any others that want to remove the link should check 
	 the online FAQ for instructions on how to obtain a version without the link -->
<!-- Removing this link will make the script stop from working -->
<div style="position:absolute; top:0; left:0; "><table border=0><tr><td><font size=-2><a style="font-size:7pt;text-decoration:none;color:silver" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font></td></tr></table></div>

<!-- Build the browser's objects and display default view of the 
     tree. -->
<script>
initializeDocument()
//Click the Parakeet link
loadSynchPage(506027036)
</script>
<noscript>
A tree for site navigation will open here if you enable JavaScript in your browser.
</noscript>

</body>

</html>
