//d = new Date();
//aogUpdater = {
//  lastchecked: d.getTime(),
//
//  check: function (since) {
//    $.getJSON(Drupal.settings.basePath + '/archimedes-og/activity-update.json?since=' + this.lastchecked + '&gid=' + Drupal.settings.og.group_context.nid, function(data) {
//      $('table.archimedes-activity-table tbody:first').prepend(data.html);
//      aogUpdater.lastchecked = data.timestamp;
//      var t = setTimeout("aogUpdater.check()", 60000);
//    });
//  }
//};

$(document).ready(function() {
  $('.archimedes-event').addClass('archimedes-js');

  // For content that is higher than 100px, shorten the height
  // so that the scrolling is not so bad.
  $('.archimedes-event .comment, .archimedes-event .activity').each(function() {
    $(this).addClass('height-' + $(this).height());
    if ($(this).hasClass('comment')) {
      var $height = 100;
    }
    else {
      var $height = 110;
    }
    if ($(this).height() > $height) {
      $(this).addClass('shortened');
      $(this).css({
        height: $height + "px",
        overflow: "hidden"
      });
    }
  });

  // Content that is shortened in height will require
  // js effects so that all content can be viewed when focused on.
  $('.shortened').hoverIntent(function(){
    var $height;
    $.each($(this).attr('class').split(' '), function() {
      if (this.substr(0, 6) == 'height') {
        $height = this.substr(7);
      }
    });
    if ($height) {
      $(this).animate({
        height: $height
      });
    }
  }, function () {
    if ($(this).hasClass('comment')) {
      var $height = 100;
    }
    else {
      var $height = 110;
    }
    $(this).animate({
      height: $height + "px"
    });
  });

  // Only show 2 comments by default, this prevents posts from
  // being too long and extending scroll length.
  $('.archimedes-js .activity-comments').each(function () {
    if ($(this).find('.comment').length < 2) {
      return;
    }
    var limit = $(this).find('.comment').length - 2;
    if (!limit) {
      return;
    }
    $(this).find('.comment').each(function (idx, el) {
      if (idx >= limit) {
        return;
      }
      $(el).addClass('hidden').hide();
    });
    $(this).prepend('<div class="comment more"><a href="#" class="show-all">' + limit + ' more</a></div>'); 
  });

  // If a user is interested in seeing more comments, they can click
  // this link which will show all the comments.
  $('.more a.show-all').live('click', function(event) {
    event.stopPropagation();
    event.preventDefault();
    $(this).parent().parent().find('.hidden').show().removeClass('hidden');
    $(this).parent().remove();
  });

//  aogUpdater.check();
});
