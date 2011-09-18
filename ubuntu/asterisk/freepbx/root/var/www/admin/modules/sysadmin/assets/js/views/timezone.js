$(document).ready(function() {
	//set child dropdown if it isnt already shown
	set_child($('#sel_area').val());
	
	//show child dropdown on zone select
	$('#sel_area').bind('blur click change keypress', function(){
		set_child($(this).val());
	})
	
	
	//submit form
    $('input[type=submit]').click(function(e){
		freepbx_info_bar('Please wait while your action is proccesed', '10000');
		var form = $(this).parents('form');
		$.ajax({
		    type: "POST",
		    url: form.attr('action'),
		    data: form.serialize(),
		    error: function(xhr, status, error) {
				freepbx_alert_bar('An error has occurred, please try again', '10000');
		     },
		    success: function(response) {
				//once weve confimed the request, wait five seconds 
				//so that apache has a chance to start up again
				setTimeout("location.reload(true);",5000);
		    }
		});
		return false;
	});

});

function set_child(zone) {
	//hide all children and show just the child mathicng the parrent
	$('.sel_zone').hide().each(function(){
		if ($(this).attr('data-zone') == zone) {
			$(this).show();
		}
	})
}