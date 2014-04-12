<?

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
	var topOffset = 150;
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
			    el.style = "";
			    el.style.left = (node.x-25)+"px";
			    el.style.top = (node.y-25+topOffset)+"px";
			    el.onclick = function(id) {
			        return function() { $("hover_"+id).toggle(); };
			    }(node.id);
			    var p = document.createElement("p");
			    el.appendChild(p);
			    p.innerHTML = node.id;
			    var hover = document.createElement("div");
			    el.appendChild(hover);
			    hover.id = "hover_"+node.id;
			    hover.className = "nodeHover";
			    hover.style = "display:none;";
			    hover.style.left = (50)+"px";
			    hover.style.top = (50)+"px";

			    $(hover).insert("<p>"+node.manufacturerName+"</p>");
			    $(hover).insert("<p>"+node.productName+"</p>");
			    $(hover).insert("<p>Status: "+(node.isFailed ? "FAILED" : "ok")+"</p>");
			    $(hover).insert("<p>Awake: "+(node.isAwake ? "Yes" : "No")+"</p>");
			    $(hover).insert("<p>OpenZWave query stage: "+node.queryStage+"</p>");

			    $(hover).insert("<p>LinuxMCE Devices</p>");
			    var s = "";
			    for (var j = 0; j < node.devices.length; j++) {
				var device = node.devices[j];
			        s += "<li>"+device.name+" #"+device.pkDevice+"("+device.fkDeviceTemplate+")</li>";
			    }
			    $(hover).insert("<ul>"+s+"</ul>");
			    var stats = node.statistics;
			    $(hover).insert("</ul><p>Detailed statistics:</p>");
			    $(hover).insert("<p>Message count:</p>");
			    $(hover).insert("<p>Sent: " + stats.sentCnt + ", failed: " + stats.sentFailed + ", retries: " + stats.retries + "</p>");
			    $(hover).insert("<p>Received: " + stats.receivedCnt + ", duplicates: " + stats.receivedDups + ", unsolicited: " + stats.receivedUnsolicited + "</p>");
			    $(hover).insert("<p>RTT (round-trip-time)</p>");
			    $(hover).insert("<p>Last request: " + stats.lastRequestRTT + ", average: " + stats.averageRequestRTT + "</p>");
			    $(hover).insert("<p>Last response: " + stats.lastResponseRTT + ", average: " + stats.averageResponseRTT + "</p>");
			    $(hover).insert("<p>Last sent: " + stats.sentTS + "</p>");
			    $(hover).insert("<p>Last received: " + stats.receivedTS + "</p>");
			    $(hover).insert("<p>Quality: " + stats.quality + "</p>");

			    $(\'nodedisplay\').appendChild(el);
			}
			// Draw lines between nodes
			for (var i = 0; i < nodes.length; i++) {
			    updateLinksForNode(nodes[i].id);
			}
			$$(\'div[class="node"]\').invoke("observe", "mousedown", function(event) { mouseDownNode(event) });
		    }
		   });
		}
		function getNode(id) {
		    var i = 0;
	 	    while (i < nodes.length) {
		        if (nodes[i].id == id)
			    return nodes[i];
		        i++;
		    }
		}
		function updateLinksForNode(id) {
		    var node = getNode(id)
		    var neighbors = node.neighbors;
		    for (var j = 0; j < neighbors.length; j++) {
		    	updateLink(id, neighbors[j]);
		    }			    
		}
		function updateLink(fromNodeId, toNodeId) {
		    var reverse = false;
		    var linkEl = $(\'link_\'+fromNodeId+\'_\'+toNodeId);
		    if (!linkEl) {
		        // look for link the other way around
		        linkEl = $(\'link_\'+toNodeId+\'_\'+fromNodeId);
			if (!linkEl) {
		            // create new link
		   	    linkEl = document.createElement("div");
			    linkEl.id = \'link_\'+fromNodeId+\'_\'+toNodeId;
			    linkEl.className = "link";
			    // Create two halves that we can color to indicate direction
		   	    var halfEl = document.createElement("div");
			    halfEl.id = linkEl.id + "_" + toNodeId;
			    halfEl.className = "link_bad linkhalf";
			    halfEl.top = "50%";
			    linkEl.appendChild(halfEl);
		   	    halfEl = document.createElement("div");
			    halfEl.id = linkEl.id + "_" + fromNodeId;
			    halfEl.className = "link_ok linkhalf";
			    halfEl.top = "0px";
			    linkEl.appendChild(halfEl);

			    $(\'nodedisplay\').appendChild(linkEl);
			} else {
			    reverse = true;
			    var halfElFromMe = $(linkEl.id+"_"+fromNodeId);
			    halfElFromMe.className = "link_ok link_half";
			}
		    }
		    if (reverse) {
		        var tmp = fromNodeId;
			fromNodeId = toNodeId;
			toNodeId = tmp;
		    }
		    // make sure the link indicates a link from fromNode to toNode
		    var fromNode = getNode(fromNodeId);
		    linkEl.style.top = (fromNode.y+topOffset)+"px";
		    linkEl.style.left = fromNode.x+"px";
		    var toNode = getNode(toNodeId);

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
			    movingNode = event.target;
			}
			function mouseUpNode(event) {
			    if (isMouseDown) {
			        isMouseDown = false;
			        var id = movingNode.id;
			        movingNode = null;
				if (hasMoved)
			            updateNodePositions(id, nodePos.x, nodePos.y);
			    }
			}
			function mouseMoveNode(event) {
			    if (isMouseDown) {
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
            
	    document.observe("dom:loaded", function() {
	        loadData();
	        $$(\'body\').invoke("observe", "mouseup", function(event) { mouseUpNode(event) });
	        $$(\'body\').invoke("observe", "mousemove", function(event) { mouseMoveNode(event) });
	    });
</script>
<style>
.node { position:absolute; width: 50px; height: 50px; background: grey; -moz-border-radius: 25px; -webkit-border-radius: 25px; border-radius: 25px;  z-index: 20;}
.node p { text-align: center; vertical-align: middle;}
.nodeHover { width: 300px; position:absolute; background: #CCCC66; border: 1px solid black; margin: 2px; z-index:100;}
.nodeHover p { text-align: left; line-spacing: 0.5; margin-top: 2px; margin-bottom: 2px; }
.nodeHover ul { margin: 2px; }
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

</style>
';
	$out.='<div id="nodedisplay"></div>';

    } else {
        // Ajax
	if (isset($_GET['getData'])) {
	    header('Content-type: application/json');
        
	    $cmd='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 1754 1 870';
	    $ret=exec_batch_command($cmd,1);
	    $retArray=explode("\n",$ret);
	    if ($retArray[0].strrpos("OK") >= 0) {
	        $floorplanInfo=getDeviceData($pkZWave,10,$dbADO);
	        if ($floorplanInfo == "") {
	            $floorplanInfo = "{\"nodes\": []}";
	        }
	        $o = json_decode($floorplanInfo);
		$fpInfo = $o->nodes;
	        $json = substr($retArray[1], 4, $retArray[1].length-7);
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
    }




    $out.='</p>';
    $out.='</form>';

    $output->setBody($out);
    $output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
    $output->setPageTitle("ZWave");
    $output->setNavigationMenu(array('Advanced ZWave'=>'index.php?section=advancedZWave'));
    $output->setTitle(APPLICATION_NAME);
    $output->output();
}

?>
