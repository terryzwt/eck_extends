(function ($) {
  Drupal.behaviors.eckextend = {
    attach: function (context) {
      if ('datepicker' in Drupal.settings.eckextend) {
        $.each(Drupal.settings.eckextend.datepicker, function (index, value) {
          $(value.class).datepicker(value.settings);
        });
      }

      if ('datetimepicker' in Drupal.settings.eckextend) {
        $.each(Drupal.settings.eckextend.datetimepicker, function (index, value) {
          $(value.class).datetimepicker();
        });
      }
    }
  };
})(jQuery);
