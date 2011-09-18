$(document).ready(function() {
    //hide settings if were are not using ups

	if ($('input[name="server"]:checked').val() == '1') {
            $('#ups').show();
        } else {
            $('#ups').hide();
        }

    $('[name=server]').click(function(){
        if ($('input[name="server"]:checked').val() == '1') {
            $('#ups').slideDown();
        } else {
            $('#ups').slideUp();
        }
	});
	
	if ($('input[name="common"]:checked').val() == 'on') {
		$('#common').show();
		$('#separate').hide();
	} else {
		$('#common').hide();
		$('#separate').show();
	}
	
	$('[name=common]').click(function(){
		if ($('input[name="common"]:checked').val() == 'on') {
			$('#separate').slideUp();
			$('#common').slideDown();
		} else {
			$('#separate').slideDown();
			$('#common').slideUp();
		}
	});
    
	
	$('[name=ups_type]').change(function(){

		switch(document.ups.ups_type.value) {
			case '':
				$('.example').text('');
				break;
			case 'USB':
				$('.example').text('Leave Blank for Autodetect.');
				break;
			case 'APCSmart':
				$('.example').text('Example: /dev/tty**');
				break;
			case 'Network':
				$('.example').text('Example: hostname:port');
				break;
			case 'SNMP':
				$('.example').text('Example: hostname:port:vendor:community');
				break;
			case 'Dumb':
				$('.example').text('Example: /dev/tty**');
				break;
		}
		
	});
});