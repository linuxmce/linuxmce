<?php

function advancedZWave($output,$dbADO){
    $out = "<script>
    </script>";

	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	$installationID = (int)@$_SESSION['installationID'];

     $action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

     $pkZWave = getDeviceFromDT($installationID,1754,$dbADO);
     
    if($action=='form'){
        $out.='<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
<script>
	var nodes;
	var isMouseDown = false;
	var hasMoved = true;
	var movingNode;
	var nodePos = new Object();
	var topOffset = 180;
        var lastStatusId = -1;
        var isStatusOn = false;
	    function loadData() {
	        new Ajax.Request("index.php", {
		    method:"get",
		    parameters:{section: "advancedZWave", action:"ajax", getData:"1" },
		    onSuccess: function(transport) {
		  	var response = transport.responseText || "";
		  	var json;
			try {			
			    json = response.evalJSON(true);
			} catch (a) {
			    alert(a);
			}
			nodes = json.nodes;
			for (var i = 0; i < nodes.length; i++) {
			    var el = document.createElement("div");
			    var node = nodes[i];
			    el.id = node.id;
			    el.className = "node";
			    if (node.isFailed) {
			        el.className += " failed";
			    } else if (!node.isAwake) {
			        el.className += " sleeping";
			    }
			    el.style = "";
			    el.style.left = (node.x-25)+"px";
			    el.style.top = (node.y-25+topOffset)+"px";

			    $(\'nodedisplay\').appendChild(el);

			    var el2 = document.createElement("div");
			    el2.className = "nodeHandle";
			    el2.style = "padding-top:1px; height: 100%;";
			    el2.onclick = function(id) {
			        return function(event) { $("hover_"+id).style.left=( document.getElementById(id).offsetLeft+50)+"px"; $("hover_"+id).style.top = document.getElementById(id).offsetTop+"px"; $("hover_"+id).toggle();};
			    }(node.id);
			    el2.insert("<p>"+node.id+"</p>");
			    el.appendChild(el2);

			    var hover = document.createElement("div");
			    $(\'nodedisplay\').appendChild(hover);
			    hover.id = "hover_"+node.id;
			    hover.className = "nodeHover";
			    hover.style.left = (50)+"px";
			    hover.style.top = (50)+"px";
			    hover.style.display = "none";

			    $(hover).insert("<p>"+node.manufacturerName+"</p>");
			    $(hover).insert("<p>"+node.productName+"</p>");
			    $(hover).insert("<span class=\"tab\" onclick=\"selectTab(\'status\',"+node.id+")\">Status</span>");
			    $(hover).insert("<span class=\"tab\" onclick=\"selectTab(\'command\',"+node.id+")\">Commands</span>");
			    $(hover).insert("<span class=\"tab\" onclick=\"selectTab(\'config\',"+node.id+")\">Configuration</span>");
			    $(hover).insert("<span class=\"tab\" onclick=\"selectTab(\'values\',"+node.id+")\">Values</span>");
			    $(hover).insert("<span class=\"tab\" onclick=\"selectTab(\'association\',"+node.id+")\">Associations</span>");
			    var tabSectionEl = document.createElement("div");
			    tabSectionEl.id = "tabsection";
			    $(hover).appendChild(tabSectionEl);

			    var dt = document.createElement("div");
			    dt.id = "statusTab_"+node.id;
			    tabSectionEl.appendChild(dt);
			    dt.insert("<p>Status: "+(node.isFailed ? "FAILED" : "ok")+"</p>");
			    if (node.isFailed) {
			    	    dt.insert("<p class=\"command\" onclick=\"replaceNode("+node.id+");\">Replace node</p>");
			    	    dt.insert("<p class=\"command\" onclick=\"removeFailedNode("+node.id+");\">Remove node</p>");
			    }

			    dt.insert("<p>Awake: "+(node.isAwake ? "Yes" : "No")+"</p>");
			    dt.insert("<p>OpenZWave query stage: "+node.queryStage+"</p>");

			    dt.insert("<p>LinuxMCE Devices</p>");
			    var s = "";
			    for (var j = 0; j < node.devices.length; j++) {
				var device = node.devices[j];
			        s += "<li>"+device.name+" #"+device.pkDevice+"("+device.fkDeviceTemplate+")</li>";
			    }
			    dt.insert("<ul>"+s+"</ul>");
			    var stats = node.statistics;
			    dt.insert("</ul><p>Detailed statistics:</p>");
			    dt.insert("<p>Message count:</p>");
			    dt.insert("<p>Sent: " + stats.sentCnt + ", failed: " + stats.sentFailed + ", retries: " + stats.retries + "</p>");
			    dt.insert("<p>Received: " + stats.receivedCnt + ", duplicates: " + stats.receivedDups + ", unsolicited: " + stats.receivedUnsolicited + "</p>");
			    dt.insert("<p>RTT (round-trip-time)</p>");
			    dt.insert("<p>Last request: " + stats.lastRequestRTT + ", average: " + stats.averageRequestRTT + "</p>");
			    dt.insert("<p>Last response: " + stats.lastResponseRTT + ", average: " + stats.averageResponseRTT + "</p>");
			    dt.insert("<p>Last sent: " + stats.sentTS + "</p>");
			    dt.insert("<p>Last received: " + stats.receivedTS + "</p>");
			    dt.insert("<p>Quality: " + stats.quality + "</p>");

			    var ct = document.createElement("div");
			    ct.id = "commandTab_"+node.id;
			    ct.style.display = "none";
			    tabSectionEl.appendChild(ct);
			    ct.insert("<p class=\"command\" onclick=\"healNode("+node.id+");\">Heal node</p>");
			    ct.insert("<p class=\"command\" onclick=\"updateNodeNeighbors("+node.id+");\">Update node neighbors</p>");
			    ct.insert("<p class=\"command\" onclick=\"testNode("+node.id+");\">Test node</p>");

			    // Config and Values tab
			    var vt = document.createElement("div");
			    vt.id = "valuesTab_"+node.id;
			    vt.style.display = "none";
			    tabSectionEl.appendChild(vt);
			    vt.insert("<p>Values</p>");
			    var t = "<table><tr>";
			    t += "<td>Index</td><td>Label</td><td>Value</td><td>Units</td><td>Genre</td><td>PK_Device</td><td>Polled</td></tr>";

			    var configEl = document.createElement("div");
			    configEl.id = "configTab_"+node.id;
			    configEl.style.display = "none";
			    tabSectionEl.appendChild(configEl);
			    var configt = "<table><tr>";
			    configt += "<td>Index</td><td>Label</td><td>Value</td><td>Units</td></tr>";

			    for (var j = 0; j < node.values.length; j++) {
				var value = node.values[j];
				var tooltip = value.help;
				var row = "<tr><td>"+value.index+"</td>";
				var onclick = "";
				if (value.genre == "config") {
				    onclick = "setConfigParam("+node.id+","+value.index+")";
				}
				row += "<td class=\"cell_"+value.genre+"\"onclick=\""+onclick+"\" title=\""+tooltip+"\">"+value.label+"</td>";
				row += "<td>"+value.value+"</td>";
				row += "<td>"+value.units+"</td>";
				if (value.genre != "config") {
				    row += "<td>"+value.genre+"</td>";
				    row += "<td>"+value.pk_device+"</td>";
				    row += "<td><input type=\"checkbox\" " + (value.polling ? "checked=\"checked\" " : "") + "onchange=\"changePolling("+value.pk_device+",\'"+value.label+"\',this.checked);\"></td>";
				}
				row += "</tr>";
				if (value.genre == "config") {
				    configt += row;
				} else {
				    t += row;
				}
			    }
			    t += "</table>";
			    configt += "</table>";
			    vt.insert(t);
			    configEl.insert(configt);

			    var assocEl = document.createElement("div");
			    assocEl.id = "associationTab_"+node.id;
			    assocEl.style.display = "none";
			    tabSectionEl.appendChild(assocEl);
			    var assoct = "<table><tr>";
			    assoct += "<td>Group</td><td>Nodes</td><td>Add</td><td>Remove</td></tr>";

			    for (var j = 0; j < node.associationGroups.length; j++) {
				var group = node.associationGroups[j];
				assoct += "<tr><td>"+group.name+":</td>";
				var assocNodes = "";
				for (var a = 0; a < group.nodes.length; a++) {
				    assocNodes += group.nodes[a] + ",";
				}
				if (group.nodes.length > 0) {
				    assocNodes = assocNodes.substr(0, assocNodes.length - 1);
				}
				assoct += "<td>"+assocNodes+"</td>";
				assoct += "<td><p class=\"command\" onclick=\"addAssociation("+node.id+","+(j+1)+");\">Add</p></td>";
				assoct += "<td><p class=\"command\" onclick=\"removeAssociation("+node.id+","+(j+1)+");\">Remove</p></td></tr>";
			    }
			    assoct += "</table>";
			    assocEl.insert(assoct);

			}
			$$(\'.nodeHandle\').invoke("observe", "mousedown", function(event) { mouseDownNode(event) });
			// Draw lines between nodes
			for (var i = 0; i < nodes.length; i++) {
			    updateLinksForNode(nodes[i].id);
			}
		    }
		   });
		}
		function showStatus(start, clearOnStart) {
		  if (start || isStatusOn) {
		      if (clearOnStart && !isStatusOn) {
		          var listEl = $("statusList");
		          while (listEl.firstChild) {
		              listEl.removeChild(listEl.firstChild);
                          }
		      }
		      isStatusOn = true;
		      $("statusArea").show();
		      new Ajax.Request("index.php", {
		          method:"get",
		          parameters:{section: "advancedZWave", action:"ajax", getStatus:"1" },
		          onSuccess: function(transport) {
		  	      var response = transport.responseText || "";
		  	      var json;
		   	      try {			
			          json = response.evalJSON(true);
			      } catch (a) {
			          alert(a);
			      }
			      for (var i = 0; i < json.statusList.length; i++) {
			          var stat = json.statusList[i];
				  var pel = document.createElement("p");
				  pel.className = "status";
				  pel.insert(stat.description);
			      	  $("statusList").appendChild(pel);
			      }
		         }
		    });
		    setTimeout(function(){ showStatus(false, false); }, 1000);
		  }
		}
		function selectTab(tab,id) {
		    $("statusTab_"+id).hide();
		    $("commandTab_"+id).hide();
		    $("configTab_"+id).hide();
		    $("valuesTab_"+id).hide();
		    $("associationTab_"+id).hide();
		    $(tab+"Tab_"+id).show();
		}
		function performCommand(parameters) {
		    parameters.section = "advancedZWave";
		    parameters.action = "ajax";
	            new Ajax.Request("index.php", {
		        method:"post",
		        parameters: parameters,
		        onSuccess: function(transport) {
			
			}
		        });
		
		}
		function addNode(secure) {
		    showStatus(true, true);
		    performCommand({"addNode":1, "secure": (secure ? "true" : "false")});
		}
		function removeNode() {
		    showStatus(true, true);
		    performCommand({"removeNode":1});
		}
		function removeFailedNode(id) {
		    showStatus(true, true);
		    performCommand({"removeFailedNode":1, "node":id});
		}
		function cancelControllerCommand() {
		    showStatus(true, true);
		    performCommand({"cancelControllerCommand":1});
		}
		function addAssociation(id, groupid) {
		    var node = prompt("Add association to node:");
		    performCommand({"addAssociation":id, "group":groupid, "node":node});
		}
		function removeAssociation(id, groupid) {
		    var node = prompt("Remove association to node:");
		    performCommand({"removeAssociation":id, "group":groupid, "node":node});
		}
		function healNode(id) {
		    performCommand({healNode:id});
		}
		function replaceNode(id) {
		    if (confirm("To replace a failed node, press OK, then perform the inclusion operation(se devices manual) on the new device.")) {
		        showStatus(true, true);
		        performCommand({replaceNode:id});
		    }
		}
		function testNode(id) {
		    performCommand({testNode:id});
		}
		function updateNodeNeighbors(id) {
		    showStatus(true, true);
		    performCommand({updateNodeNeighbors:id});
		}
		function setConfigParam(nodeId, index) {
		    var val = prompt("Please enter new value for parameter:");
		    if (val != "")
		        performCommand({setConfigParam:nodeId, paramIndex:index, valueToSet:val});
		}
		function changePolling(pkDevice,label,state) {
		    performCommand({"setPolling":1, "pkDevice": pkDevice, "label":label, "state": state});
		}
		function getNode(id) {
		    var i = 0;
	 	    while (i < nodes.length) {
		        if (nodes[i].id == id)
			    return nodes[i];
		        i++;
		    }
		    return null;
		}
		function updateLinksForNode(id) {
		    var node = getNode(id)
		    var neighbors = node.neighbors;
		    for (var j = 0; j < neighbors.length; j++) {
		    	updateLink(id, neighbors[j], true);
		    }
		    // Need to look for any links going just to this node (just for update, dont create new ones)
		    // the correct way (but more time-consuming) would be to iterate every nodes neighbours
		    for (var j = 0; j < nodes.length; j++) {
		        if (nodes[j].id != id)
		    	    updateLink(nodes[j].id, id, false);
		    }
		}
		function updateLink(fromNodeId, toNodeId, createNew) {
		    var reverse = false;
		    var linkEl = $(\'link_\'+fromNodeId+\'_\'+toNodeId);
		    if (!linkEl) {
		        // look for link the other way around
		        linkEl = $(\'link_\'+toNodeId+\'_\'+fromNodeId);
			if (!linkEl) {
			    if (createNew) {
		                // create new link
		   	    	linkEl = document.createElement("div");
				linkEl.id = \'link_\'+fromNodeId+\'_\'+toNodeId;
			    	linkEl.className = "link";
			    	// Create two halves that we can color to indicate direction
		   	    	var halfEl = document.createElement("div");
			    	halfEl.id = linkEl.id + "_" + fromNodeId;
			    	halfEl.className = "link_ok linkhalf";
			    	halfEl.top = "50%";
			    	linkEl.appendChild(halfEl);
		   	    	halfEl = document.createElement("div");
			    	halfEl.id = linkEl.id + "_" + toNodeId;
			    	halfEl.className = "link_bad linkhalf";
			    	halfEl.top = "0px";
			    	linkEl.appendChild(halfEl);

			    	$(\'nodedisplay\').appendChild(linkEl);
			    } else {
			        return;
			    }
			} else {
			    reverse = true;
			    var halfElFromMe = $(linkEl.id+"_"+fromNodeId);
			    halfElFromMe.className = "link_ok link_half";
		            var tmp = fromNodeId;
			    fromNodeId = toNodeId;
			    toNodeId = tmp;
			}
		    }
		    // make sure the link indicates a link from fromNode to toNode
		    var fromNode = getNode(fromNodeId);
		    linkEl.style.top = (fromNode.y+topOffset)+"px";
		    linkEl.style.left = fromNode.x+"px";
		    var toNode = getNode(toNodeId);
		    if (toNode == null)
		        return;
		    var length = Math.sqrt(Math.pow(toNode.x - fromNode.x,2) + Math.pow(toNode.y - fromNode.y,2));
		    linkEl.style.height = length + "px";

		    var angle = 180 / 3.14 * Math.acos((toNode.y - fromNode.y) / length);
		    if (toNode.x > fromNode.x)
		        angle *= -1;
		    linkEl.style.webkitTransform = "rotate(" + angle + "deg)";
		    linkEl.style.mozTransform = "rotate(" + angle + "deg)";
		    linkEl.style.msTransform = "rotate(" + angle + "deg)";
		    linkEl.style.transform = "rotate(" + angle + "deg)";
		}
		function updateNodePositions(id, x, y) {
	            new Ajax.Request("index.php", {
		        method:"post",
		        parameters:{section: "advancedZWave", action:"ajax", updateNodePosition:id, x:x, y:y },
		        onSuccess: function(transport) {
					  	   
			}
		        });
			 
		}
			function mouseDownNode(event) {
			    isMouseDown = true;
			    hasMoved = false;
			    movingNode = event.target.parentElement;
			}
			function mouseUpNode(event) {
			    if (isMouseDown && movingNode != null) {
			        isMouseDown = false;
			        var id = movingNode.id;
			        movingNode = null;
				if (hasMoved)
			            updateNodePositions(id, nodePos.x, nodePos.y);
			    }
			}
			function mouseMoveNode(event) {
			    if (isMouseDown && movingNode != null) {
			        hasMoved = true;
			        var x = event.pageX;
			        var y = event.pageY;
			        movingNode.style.left = (x-25)+"px";
			        movingNode.style.top = (y-25)+"px";
				nodePos.x = x;
				nodePos.y = y-topOffset;
				event.preventDefault();
				var node = getNode(movingNode.id);
				node.x = x;
				node.y = y-topOffset;
				updateLinksForNode(movingNode.id);
			    }
		}    
	function refresh() {
	    $(\'nodedisplay\').innerHTML="";
	    loadData();
	}            
	function healNetwork() {
	    performCommand({healNetwork:1});
	}            
	function testNetwork() {
	    performCommand({testNetwork:1});
	}            
	function networkUpdate() {
	    performCommand({testNetwork:1});
	}            
	function softReset() {
	    performCommand({softReset:1});
	}            
	    document.observe("dom:loaded", function() {
	        loadData();
	        $$(\'body\').invoke("observe", "mouseup", function(event) { mouseUpNode(event) });
	        $$(\'body\').invoke("observe", "mousemove", function(event) { mouseMoveNode(event) });
	    });
</script>
<style>
.node { position:absolute; width: 50px; height: 50px; background: grey; -moz-border-radius: 25px; -webkit-border-radius: 25px; border-radius: 25px;  z-index: 20;}
.sleeping { background-color: #FFFF00; }
.failed { background-color: red; }
.node p { text-align: center; vertical-align: middle;}
.nodeHover { width: 400px; position:absolute; background: #CCCC66; border: 1px solid black; margin: 2px; z-index:1000;}
.nodeHover p { text-align: left; line-spacing: 0.5; margin-top: 2px; margin-bottom: 2px; }
.nodeHover ul { margin: 2px; }
span.tab { padding: 3px; padding-left: 6px; padding-right: 6px; border:1px solid black; border-bottom: 0px; margin: 5px; margin-bottom:7px; }
span.tab:hover { background-color: white; }

.link { position: absolute; width: 3px; background-color: red; z-index: 10; -webkit-transform-origin: top left;
    -moz-transform-origin: top left;
    -o-transform-origin: top left;
    -ms-transform-origin: top left; }
.linkhalf:hover {
    background-color: blue;
}
.link_ok {
    background-color: green;
    width: 100%;
    height: 50%;
}
.link_bad {
    background-color: red;
    width: 100%;
    height: 50%;
}
.command {
    background-color: grey;
    border: 1px solid black;
    margin: 2px;
    padding: 2px;
}
.command:hover {
    background-color: white;
}
td.cell_config:hover {
    background-color: white;  
}
#tabSection {
    border-top: 1px solid black;
    margin: 0;
    margin-top: 2px;
}
#statusArea {
    position: absolute;
    width: 350px;
    left: 200px;
    top: 250px;
    border: 1px solid black;
    background-color: grey;
    padding: 3px;
    z-index: 1000;
}
p.status {
    margin: 1px;
}
</style>
';
	$out.='<input class="command" type="button" value="Refresh" onclick="refresh()" />';
	$out.='<input class="command" type="button" value="Heal network" onclick="healNetwork()" />';
	$out.='<input class="command" type="button" value="Network Update" onclick="networkUpdate()" />';
	$out.='<input class="command" type="button" value="Test network" onclick="testNetwork()" />';
	$out.='<input class="command" type="button" value="Soft reset controller" onclick="softReset()" />';
	$out.='<input class="command" type="button" value="Add Node" onclick="addNode(true)" />';
	$out.='<input class="command" type="button" value="Remove Node" onclick="removeNode()" />';
	$out.='<input class="command" type="button" value="Cancel Add/Remove" onclick="cancelControllerCommand()" />';
	$out.='<input class="command" type="button" value="Add Node(unsecure)" onclick="addNode(false)" />';
	$out.='<input class="command" type="button" value="Show Status" onclick="showStatus(true, false)" />';
	$out.='<div id="nodedisplay"></div>';
	$out.='<div id="statusArea" style="display:none;">
<div><span><strong>Status messages from ZWave:</strong></span>
<span style="float: right" onclick="$(\'statusArea\').hide(); isStatusOn = false;">[Close]</span></div>
<div id="statusList"></div>
</div>';

    } else {
        // Ajax
	if (isset($_GET['getData'])) {
	    header('Content-type: application/json');
        
	    $cmd='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 1754 1 870';
	    $ret=exec_batch_command($cmd,1);
	    $retArray=explode("\n",$ret);
	    if (strrpos($retArray[0], "OK") >= 0) {
	        $floorplanInfo=getDeviceData($pkZWave,10,$dbADO);
	        if ($floorplanInfo == "") {
	            $floorplanInfo = "{\"nodes\": []}";
	        }
	        $o = json_decode($floorplanInfo);
		$fpInfo = $o->nodes;

		$startPos = strpos($retArray[1],"{");
	        $json = substr($retArray[1], $startPos, strrpos($retArray[1],"}")-$startPos+1);
		$o = json_decode($json);
		$nodes = $o->nodes;

		$deviceNames = getDeviceNames($dbADO);

		// Set x & y
		$x = 50;
		$y = 50;
		foreach ($nodes as $n) {
		    $found = false;
		    $i = 0;
	            while (!$found && $i < count($fpInfo)) {
	                if ($fpInfo[$i]->id == $n->id) {
		    	    $found = true;
		    	    $n->x = $fpInfo[$i]->x;
			    $n->y = $fpInfo[$i]->y;
			}
		        $i++;
	            }
		    if (!$found) {
		        $n->x = $x;
		        $n->y = $y;
			$x += 100;
			if ($x > 1024) {
			    $x = 50;
			    $y+=100;
			}
		    }
		    // Get device names
		    foreach ($n->devices as $device) {
		        $device->name = $deviceNames[$device->pkDevice];
		    }

		}
                print(json_encode($o));
		exit();
	    }
        } else if (isset($_GET['getStatus'])) {
	    header('Content-type: application/json');
            $cmd='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 1754 1 870 9 "status"';
	    $ret=exec_batch_command($cmd,1);
	    $retArray=explode("\n",$ret);
	    if ($retArray[0].strrpos("OK") >= 0) {
		$startPos = strpos($retArray[1],"{");
	        $json = substr($retArray[1], $startPos, strrpos($retArray[1],"}")-$startPos+1);
		$o = json_decode($json);
                print(json_encode($o));
		exit();
	    }
	}
	if (isset($_POST['updateNodePosition'])) {
	    header('Content-type: application/json');
	    $id = $_POST['updateNodePosition'];
	    if ($id == "")
	        exit();
	    $y = (int)$_POST['y'];
	    $x = (int)$_POST['x'];

	    $floorplanInfo=getDeviceData($pkZWave,10,$dbADO);
	    if ($floorplanInfo == "") {
	        $floorplanInfo = "{\"nodes\": []}";
	    }
	    $o = json_decode($floorplanInfo);
	    $nodes = $o->nodes;
	    $found = false;
	    $i = 0;
	    while (!$found && $i < count($nodes)) {
	        if ($nodes[$i]->id == $id) {
		    $found = true;
		    $nodes[$i]->x = $x;
		    $nodes[$i]->y = $y;
		}
		$i++;
	    }
	    if (!$found) {
	        $newNode = new stdClass();
		$newNode->id = $id;
		$newNode->x = $x;
		$newNode->y = $y;
	        $nodes[] = $newNode;
	    }
	    $o->nodes = $nodes;
	    set_device_data($pkZWave,10,json_encode($o),$dbADO);
	    print(json_encode($o));
	    exit();
        }
	if (isset($_POST['healNode'])) {
	    header('Content-type: application/json');
            $id = $_POST['healNode'];
	    $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 788 51 "HNN'.$id.'"';
	    $ret=exec_batch_command($cmd,1);
	    $retArray=explode("\n",$ret);
	    print("{ \"ok\": ".($retArray[0].strrpos("OK") >= 0 ? 1 : 0)." }");
	    exit;
	}
	if (isset($_POST['testNode'])) {
	    header('Content-type: application/json');
            $id = $_POST['testNode'];
	    $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 788 51 "TNN'.$id.'"';
	    $ret=exec_batch_command($cmd,1);
	    $retArray=explode("\n",$ret);
	    print("{ \"ok\": ".($retArray[0].strrpos("OK") >= 0 ? 1 : 0)." }");
	    exit;
	}
	if (isset($_POST['softReset']) || isset($_POST['healNetwork']) || isset($_POST['testNetwork']) || isset($_POST['networkUpdate']) || isset($_POST['addAssociation']) || isset($_POST['removeAssociation']) || isset($_POST['addNode']) || isset($_POST['removeNode']) || isset($_POST['cancelControllerCommand']) || isset($_POST['setPolling']) || isset($_POST['replaceNode']) || isset($_POST['removeFailedNode'])) {

	    header('Content-type: application/json');

	    if (isset($_POST['softReset'])) {
	        $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 776 51 ""';
 	    } else if (isset($_POST['healNetwork'])) {
	        $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 788 51 "HN"';
 	    } else if (isset($_POST['testNetwork'])) {
	        $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 788 51 "TN"';
 	    } else if (isset($_POST['networkUpdate'])) {
	        $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 788 51 "NU"';
	    } else if (isset($_POST['addAssociation'])) {
	        $id = $_POST['addAssociation'];
	        $group = $_POST['group'];
	        $node = $_POST['node'];
                $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 842 239 '.$id.' 249 '.$group.' 250 '.$node;
	    } else if (isset($_POST['removeAssociation'])) {
	        $id = $_POST['removeAssociation'];
	        $group = $_POST['group'];
	        $node = $_POST['node'];
                $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 842 239 '.$id.' 249 '.$group.' 250 -'.$node;
	    } else if (isset($_POST['addNode'])) {
                $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 967 39 '.($_POST['secure'] == 'true' ? 'S' : '" "');
	    } else if (isset($_POST['cancelControllerCommand'])) {
                $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 967 48 5';
	    } else if (isset($_POST['removeNode'])) {
                $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 968';
	    } else if (isset($_POST['removeFailedNode'])) {
	        $id = (int)$_POST['replaceNode'];
                $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 968 48 '.$id;
	    } else if (isset($_POST['setPolling'])) {
	        $pkDevice = $_POST['pkDevice'];
		$label = $_POST['label'];
		$state = $_POST['state'];
                $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 966 2 '.$pkDevice.' 5 "'.$label.'" 220 "'.($state == "true" ? '1' : '').'" 225 1';
	    } else if (isset($_POST['replaceNode'])) {
	        $id = $_POST['replaceNode'];
	    	$cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 788 51 "RFN'.$id.'"';
	    }
	    $ret=exec_batch_command($cmd,1);
	    $retArray=explode("\n",$ret);
	    print("{ \"ok\": ".($retArray[0].strrpos("OK") >= 0 ? 1 : 0)." }");
	    exit;
	}
	if (isset($_POST['updateNodeNeighbors'])) {
	    header('Content-type: application/json');
            $id = $_POST['updateNodeNeighbors'];
	    $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 788 51 "NNU'.$id.'"';
	    $ret=exec_batch_command($cmd,1);
	    $retArray=explode("\n",$ret);
	    print("{ \"ok\": ".($retArray[0].strrpos("OK") >= 0 ? 1 : 0)." }");
	    exit;
	}
	if (isset($_POST['setConfigParam'])) {
	    header('Content-type: application/json');
            $id = $_POST['setConfigParam'];
            $param = $_POST['paramIndex'];
            $value = $_POST['valueToSet'];
	    $cmd='/usr/pluto/bin/MessageSend localhost 0 '.$pkZWave.' 1 841 48 "'.$value.'" 222 "0" 239 "'.$id.'" 248 "'.$param.'"';
	    $ret=exec_batch_command($cmd,1);
	    $retArray=explode("\n",$ret);
	    print("{ \"ok\": ".($retArray[0].strrpos("OK") >= 0 ? 1 : 0)." }");
	    exit;
	}
    }




    $out.='</p>';
    $out.='</form>';

    $output->setBody($out);
    $output->setMenuTitle($TEXT_AUTOMATION_CONST.' |');
    $output->setPageTitle("ZWave");
    $output->setNavigationMenu(array('Advanced ZWave'=>'index.php?section=advancedZWave'));
    $output->setTitle(APPLICATION_NAME);
    $output->output();
}

?>
