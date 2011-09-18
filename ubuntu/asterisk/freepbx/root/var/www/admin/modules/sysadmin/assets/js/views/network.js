$(document).ready(function() {
	//auto set values on page load
	set_network_props();
	
	//update options when a different interface is selected
	$('#select_network_device').change(function(e){

		//populate input fields
		set_network_props()
	}).focus(function(){//save setting on select focus
		//get values from inputs
		get_network_props()
	});
	
	//add a virtual ip
	$('#addip').click(function(){
		//hide the select dropdown and save button
		$('.ifselect').hide();	
		$('#save_set').hide();
		
		//show save interface button
		$('#save_new_if').show();
		
		//set input values
		$('input[name=ifname]').val('');
		$('input[name="usedhcp"][value="none"]').attr("checked","checked");
		//$(".radioset").buttonset('refresh');//refresh buttonsets
		$('input[name=staticip]').val('');
		$('input[name=netmask]').val('');
		$('input[name=gateway]').val('');
		$('input[name=macaddr]').val('');
		
		//enable editing of the interface name
		$('input[name=ifname]').removeAttr('disabled');
		
		//hide add button
		$(this).hide();
		
		//show delete button
		$('#delip').show();
		return false;
	});
	
	$('#save_new_if').click(save_new_if);
	//delete ip
	$('#delip').click(function(){
		//dont delete anything if nothing had been created
		if (!$('#save_new_if').is(':visible')) {
			//get currently selected interface
			iface = $('input[name=ifname]').data('name');

			//mark interface for deletion
			if (typeof iface != 'undefined') {
				//network[iface].ACTION = 'delete';
				delete network[iface];
				//remove option from select
				$('#select_network_device > option[value="' + iface + '"]').remove();
			}
		} else {
			$('#save_new_if').hide();
			$('.ifselect').show();
			$('#save_set').show();
		}

		//hide "save" button 
		$('#save_new_if').hide();
		
		//show select box again
		$('#select_network_device').show();
		
		//triger a click to refresh the input boxes
		$('#select_network_device').trigger('change');
		return false;
	})
	
	//update the name of the interface if its edited
	$('input[name=ifname]')
	.focus(function(){
		//save current name to work with after change
		old_name = $('input[name=ifname]').val();
	})
	.change(function(){
		//get new name
		new_name = $('input[name=ifname]').val();
		
		console.log('old_name', old_name);
		//save values in object under new name
		network[new_name] = network[old_name];
		
		//remove old object
		delete network[old_name];
		
		//update select box
		$('#select_network_device > option[value="' + old_name + '"]').val(new_name).html(new_name + ' (virtual)');
	});
	
	//hide ip info if were using dhcp/bootp
	$('[name=bootproto]').click(function(){
		if ($('input[name="bootproto"]:checked').val() == 'none') {
			$('.static_only').show();
		} else {
			$('.static_only').hide();
		}
	})
	
	//save settings
	$('#save_set').click(function(e){
		if (!$('input[name=ifname]').val()) {
			freepbx_alert_bar('Please enter a valid Interface Name!', 5000);
			return false;
		} 
		var conf_cont = confirm("This action may cause the system to stop prossesing calls. Do you wish to continue?")
		if (conf_cont){
			freepbx_info_bar('Please wait while your action is proccesed.', '5000');
			get_network_props();
			$('#network_set_form > input[name=network]').val(JSON.stringify(network))
			var form = $(this).parents('form');
			button = "&" + e.srcElement.name + "=" + e.srcElement.value;
			$.ajax({
			    type: "POST",
			    url: form.attr('action'),
			    data: form.serialize() + button,
			    error: function(xhr, status, error) {
					freepbx_alert_bar('An error has occurred, please try again', '10000');
			     },
			    success: function(response) {
					freepbx_info_bar('You will now be redirected. If you changed the IP address used to access this page, you will need to manually browse to the new address.', '5000');
					setTimeout("location.reload(true);",5500);
				}
			});
			return false;
		} else {
			e.preventDefault();
		}
	});
	

});
var old_name = '';
function set_network_props() {
	iface = $('#select_network_device').val();
	
	if (network[iface]) {
		//set input values
		$('input[name=ifname]').val(network[iface].name);
		$('input[name=ifname]').data('name', network[iface].name);
		//presetValue = (network[iface].BOOTPROTO == 'dhcp') ? 'true' : 'false';
		$('input[name="bootproto"][value='+network[iface].BOOTPROTO+']').attr("checked","checked");
		$('input[name="onboot"][value='+network[iface].ONBOOT+']').attr("checked","checked");
		//$(".radioset").buttonset('refresh');//refresh buttonsets
		$('input[name=staticip]').val(network[iface].IPADDR);
		$('input[name=netmask]').val(network[iface].NETMASK);
		$('input[name=gateway]').val(network[iface].GATEWAY);
		$('input[name=macaddr]').val(network[iface].HWaddr);
	
		//show add button if were dealing with a physical interface
		if (network[iface].type == 'physical') {
			//make sure users cant edit the Interface name
			$('input[name=ifname]').attr('disabled', true);
			
			//hide the delete button
			$('#delip').hide();
			
			//show the add button
			$('#addip').show();
		} else {
			//enable editing of the interface name
			$('input[name=ifname]').removeAttr('disabled');
			
			//hide add button
			$('#addip').hide();
			
			//show delete button
			$('#delip').show();
		}
	}
}

function get_network_props(iface) {
	if (typeof iface == 'undefined') {
		iface 					= $('#select_network_device').val();
	}
	network[iface] 				= network[iface] || {}
	network[iface].name			= $.trim($('input[name=ifname]').val());
	network[iface].BOOTPROTO 	= $.trim($('input[name="bootproto"]:checked').val());
	network[iface].ONBOOT	 	= $.trim($('input[name="onboot"]:checked').val());
	network[iface].IPADDR		= $.trim($('input[name=staticip]').val());
	network[iface].NETMASK		= $.trim($('input[name=netmask]').val());
	network[iface].GATEWAY		= $.trim($('input[name=gateway]').val());
	network[iface].HWaddr		= $.trim($('input[name=macaddr]').val());

}

function save_new_if() {
	if (!$('input[name=ifname]').val()) {
		freepbx_alert_bar('Please enter a valid Interface Name!', 5000);
	} else {
		iface = $('input[name=ifname]').val();
		$('input[name=ifname]').data('name', iface);
		get_network_props(iface);
		network[iface].type	= 'virtual';
		$('#select_network_device').append(new Option(iface + ' (virtual)', iface, '', true));
		$('#save_new_if').hide();
		$('.ifselect').show();
		$('#save_set').show();
		return false;
	}

}