// $Id: scripts.js,v 1.1 2009/08/20 17:49:47 davyvandenbremt Exp $

$(document).ready(function() {

  // Hide/show field type/value on field mapping form.
  $('form#campaign-monitor-admin-field-mapping-form select.account_field_type').each(function() {
    campaign_monitor_toggle_field_type(this);
  });
  $('select.account_field_type').change(function() {
    campaign_monitor_toggle_field_type(this);
  });
});

/**
 * Hide/show field type/value on field mapping form.
 *
 * @param $type
 *   Object; jQuery object for select field which change value should be used.
 */
function campaign_monitor_toggle_field_type(object) {
  var context = $(object).parents('tr');
  if ($(object).val() == 'php') {
    $('.account_field', context).parents('.form-item').show();
    $('.account_field_profile', context).hide();
    $('.account_field_token', context).hide();
  }
  else if ($(object).val() == 'token') {
    $('.account_field', context).parents('.form-item').hide();
    $('.account_field_profile', context).hide();
    $('.account_field_token', context).show();
  }
  else {
    $('.account_field', context).parents('.form-item').hide();
    $('.account_field_profile', context).show();
    $('.account_field_token', context).hide();
  }
}