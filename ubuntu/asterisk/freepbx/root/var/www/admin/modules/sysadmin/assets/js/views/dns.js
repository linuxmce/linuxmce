$(document).ready(function() {	
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