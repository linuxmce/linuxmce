//submit form
$(document).ready(function(){
	$('input[type=submit]').click(function(e){
		if (confirm("This action will cause the system to stop prossesing calls. Do you wish to continue?")){
			
			if ($(this).val() == 'Reboot') {
				//promt user to make ensure there not doing something stupid
				var answer = prompt ("To reboot, please type \"reboot\":");
				if (answer != 'reboot') {
					return false;
				}
				freepbx_info_bar('System is being rebooted. Please wait at least 5 minutes before reloading this page.', '10000');
			} else {
				//prompt user to make ensure there not doing something stupid
				var answer2 = prompt ("To Power Off, please type \"poweroff\" \n NOTE: YOU WILL NEED PHYSICAL ACCESS TO THE PBX TO POWER IT BACK ON!");
				if (answer2 != 'poweroff') {
					return false;
				}
				freepbx_info_bar('System is being shutdown. You may close this page now.');
			}
			
			var form = $(this).parents('form');
			button = "&" + $(this).attr('name') + "=" + $(this).val();
			$.ajax({
			    type: "POST",
			    url: form.attr('action'),
			    data: form.serialize() + button,
			    error: function(xhr, status, error) {
					freepbx_alert_bar('An error has occurred, please try again', '10000');
			     },
			    success: function(response) {}
			});
			return false;
		} else {
		return false;			
		}
		return false;
	});
});