$(document).ready(function() {
  /* Adds the class of .complete to anything before active state*/
  var selected = 0;
	$('#block-book-0 ul li').each(function(index){
	  if ( $(this).hasClass('active-trail') ) {
	    selected = index;
	    return false; 
	  } else {
	    $(this).addClass('complete');
	  }
	});
	
  $('.fade').mosaic(); 
});