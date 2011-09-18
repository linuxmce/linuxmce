$(document).ready(function() {
    //hide settings if were are using internal smtp

	if ($('input[name="server"]:checked').val() == '1') {
            $('#ftp_server').show();
        } else {
            $('#ftp_server').hide();
        }

    $('[name=server]').click(function(){
        if ($('input[name="server"]:checked').val() == '1') {
            $('#ftp_server').slideDown();
        } else {
            $('#ftp_server').slideUp();
        }
    });
});