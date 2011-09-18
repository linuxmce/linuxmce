$(document).ready(function() {
	$('[name=cronstate]').click(function(){
		switch ($(this).val()) {
			case 'advanced':
				$('#cronsched_adv').slideDown();
				break;
			default:
				$('#cronsched_adv').slideUp();
				break;
		}
	});
	
	//scroll down status box
	$('.update_text').scrollTop($('.update_text').attr('scrollHeight'));
	
	//start live updates
	if ($('.update_status').length > 0) {
		set_update_timeout(2500);
		//set shadow if we have what to shadow
		$('.update_text_item').eq(-5).addClass('update_text_shadow')
	}

	$('.update_status').hover(
		function(){
			clearTimeout(hover_timeout);
			hover_timeout = setTimeout(function(){
				update_msg_expanded = true;

				//remove shadowing
				$('.update_text_item').each(function(){
					$(this).removeClass('update_text_shadow')
				});
			
				//get hight of div
				height = $('.update_text').attr('scrollHeight') + 10;
				//grow div
				$('.update_status').stop(true, true).animate({height:height}, 500);
				$('.update_text').stop(true, true).animate({maxHeight: height - 10,height: height - 10}, 500);
				//scrool to the bottom
				scrollto = $('.update_status').offset().top + height - $(window).height() + 30;
				$('html,body').stop(true, true).animate({ scrollTop:  scrollto}, 500);
			}, 500);
		},
		function(){
			clearTimeout(hover_timeout);
			update_msg_expanded = false;
			//$(this).height(70)
			$('.update_status').stop(true, true).animate({height:70}, 500);
			//$('.update_text').css('maxHeight', 70).height(70);
			$('.update_text').stop(true, true).animate({maxHeight: 70,height: 70}, 500,
				function(){
					//add shadowing to top item
					$('.update_text_item').eq(-5).addClass('update_text_shadow')
					
					//now srcool back to the bottom
					$('.update_text').scrollTop($('.update_text').attr('scrollHeight'));
				});
			

		}
	);
});
var hover_timeout;
var update_timeout;
var update_msg_timeout;
var update_msg_queue = new Array();
var last_update = 0;
var update_msg_expanded = false;
var add_msg_running = false;
var last_status_id;
var foo = 0

//var tt = setInterval('add_msg("running " + foo++);if(foo > 20){clearInterval(tt)}', 50);
//var tt = setInterval('add_msg("running " + foo++);if(foo > 50){clearInterval(tt)}', 500);
function set_update_timeout(interval) {
	clearTimeout(update_timeout);
	var update_timeout = setTimeout('update_status()', interval);
	return update_timeout;
}

function update_status() {
	$.ajax({
		url: window.location.href,
		type: 'post',
		data: 'ajax=status_update&last_update='+last_update,
		dataType: 'json',
		cache: false,
		success: function(msg, status){
			for (var i in msg) {
				add = '[' + msg[i].date_string +']' + ' ' + msg[i].status_id + ': ' + msg[i].status
				update_msg_queue.push(add);
				last_update = msg[i].time;
				last_status_id = msg[i].status_id
			}
			add_msg();
			
			//850 means were done here
			if (last_status_id == 850) {
				update_msg_queue.push('Redirecting...');
				add_msg()
				setTimeout('window.location = window.location.href', 10000);
			} else {//reset timer, minimum 5 seconds
				time = update_msg_queue.length * 2500;
				time = time > 5000 ? time : 5000;
				set_update_timeout(time);
			}

		},
		error: function() {
			//todo: set error handeling
			set_update_timeout(10000);
		}
	});
}

//TODO: add_msg should be able to handel its own queue. 
//I.e. you should pass messages with add_msg(/*messages here*/) and the function whould handel the queing
function add_msg() {
	//dont run if the queue is empty or if were running already
	if (add_msg_running == true || update_msg_queue.length == 0) {
		return false;
	}
	add_msg_running = true;
	//console.log('add_msg running with ', update_msg_queue.length, ' messages in the queue', update_msg_queue)
	setTimeout(function(){
		$('.update_text').append('<div class="update_text_item">' + update_msg_queue.splice(0, 1) + '</div>')
		if (update_msg_expanded) {
			height = $('.update_text').attr('scrollHeight') + 10;
			$('.update_status').height(height);
			$('.update_text').css({'maxHeight':height-10,'height':height-10});
			//scrool page, only if we cant see lst msg
			scrollto = $('.update_status').offset().top + height - $(window).height() + 30;
			$('html, body').stop(true, true).animate({ scrollTop: scrollto}, 500);

		}				

		//add adnoird style shading to top of list (to indicate that there is more text up there)
		if ($('.update_text').attr('scrollHeight') > $('.update_status').height()) {
			$('.update_text').stop(true, true).animate({ scrollTop:  $('.update_text').attr('scrollHeight')}, 1000,
			function(){$('.update_text_item').eq(-5).addClass('update_text_shadow')});
			$('.update_text_item').eq(-6).removeClass('update_text_shadow')

		} else {
			$('.update_text_item').eq(-6).removeClass('update_text_shadow')
			$('.update_text').stop(true, true).animate({ scrollTop:  $('.update_text').attr('scrollHeight')}, 1000);
		}
		//console.log('done', update_msg_queue.length, ' messages to go!')
		add_msg_running = false;
		if(update_msg_queue.length > 0) {
			add_msg();
		}
		
	}, 2500)

}