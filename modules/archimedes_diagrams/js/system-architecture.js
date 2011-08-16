$(document).ready(function() {
  settings = Drupal.settings.archimedes_diagrams;
  $('#' + settings.struct).orgChart({
    levels : -1, 
        stack  : false,
        nodeText : function($node) {
          return $node.attr('title');
        }, 
        nodeBody : function($node) {
          var html = '';
          console.log($node);
          $node.children('.server').each(function() {
            html += $(this).html();
          });
          return html;
        },
  }, $('#' + settings.canvas)).hide();

});


