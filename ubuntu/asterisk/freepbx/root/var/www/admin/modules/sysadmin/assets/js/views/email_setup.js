$(document).ready(function() {
    //hide settings if were are using internal smtp

	if ($('input[name="server"]:checked').val() == 'external') {
            $('#ext_smtp_info').show();
        } else {
            $('#ext_smtp_info').hide();
        }

	if ($('input[name="auth"]:checked').val() == 'yes') {
            $('#auth_info').show();
        } else {
            $('#auth_info').hide();
        }
	
    $('[name=server]').click(function(){
        if ($('input[name="server"]:checked').val() == 'external') {
            $('#ext_smtp_info').slideDown();
        } else {
            $('#ext_smtp_info').slideUp();
        }
    });
	
	$('[name=auth]').click(function(){
        if ($('input[name="auth"]:checked').val() == 'yes') {
            $('#auth_info').slideDown();
        } else {
            $('#auth_info').slideUp();
        }
    });
	
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
				setTimeout("location.reload(true);",5500);
		    }
		});
		return false;
	});

});