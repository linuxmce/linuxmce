$(document).ready(function() {
    $('input[type=submit]').click(function(e){
		freepbx_info_bar('Please wait while your action is proccesed', '10000');
		//e.preventDefault();
		var form = $(this).parents('form');
		button = "&" + e.srcElement.name + "=" + e.srcElement.value;
		$.ajax({
		    type: "POST",
		    url: form.attr('action'),
		    data: form.serialize() + button,
		    error: function(xhr, status, error) {
				console.log('error')
		          //do something about the error
		     },
		    success: function(response) {
				setTimeout("location.reload(true);",10000);
		    }
		});
		return false;
	});
});
