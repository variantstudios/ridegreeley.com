<?php
// $Id: campaign_monitor.helpers.inc,v 1.1.2.6 2009/09/11 19:53:04 davyvandenbremt Exp $

/**
 * @file
 * Helper functions used by all files.
 *
 * @author Davy Van Den Bremt
 */

function campaign_monitor_helpers_parse_field_values($lid) {
  global $user;

  $values = array();

  $list = campaign_monitor_db_get_list($lid);

  foreach ($list->field_mapping as $field) {
    // php mapping
    if ($field->mapping_type == 'php') {
      $code = $field->mapping_value;
      $values[$field->key] = drupal_eval($code);
    }

    // profile mapping
    elseif ($field->mapping_type == 'profile' && module_exists('profile')) {
      profile_load_profile($user);

      if ($user->{$field->mapping_value}) {
        $values[$field->key] = $user->{$field->mapping_value};
      }
    }

    // token mapping
    elseif ($field->mapping_type == 'token' && module_exists('token')) {
      if (module_exists('profile')) {
        profile_load_profile($user);
      }

      $token_pattern = $field->mapping_value;
      $values[$field->key] = token_replace($token_pattern, 'user', $user);
    }
  }

  return $values;
}

/**
 * Subscribe a user to a list.
 *
 * @param $mail
 *   String; E-mail address to subscribe
 * @param $lid
 *   String; List ID of the list to subscribe to.
 * @param $fields
 *   Array; Array of custom field values. Key is field. Value is value for the field.
 */
function campaign_monitor_helpers_subscribe($mail, $lid, $fields) {
  campaign_monitor_db_save_request('subscribe', $mail, $lid, $fields);
  campaign_monitor_db_save_subscription($mail, $lid);
}

/**
 * Unsubscribe a user from a list.
 *
 * @param $mail
 *   String; E-mail address to unsubscribe
 * @param $lid
 *   String; List ID of the list to unsubscribe from.
 */
function campaign_monitor_helpers_unsubscribe($mail, $lid) {
  campaign_monitor_db_save_request('unsubscribe', $mail, $lid);
  campaign_monitor_db_delete_subscription($mail, $lid);
}

/**
 * Check if a certain sync action is allowed to happen.
 *
 * @param $type
 *   String; Type of sync, used in the variable name.
 * @return
 *   Boolean; TRUE if sync can happen; FALSE if not.
 */
function _campaign_monitor_allow_sync($type) {
  $frequency = variable_get('campaign_monitor_sync_'. $type .'_interval', 0);
  $last_sync = variable_get('campaign_monitor_last_sync_'. $type, 0);

  if ($frequency > -1 && ($_SERVER['REQUEST_TIME'] - $last_sync > $frequency)) {
    return TRUE;
  }

  return FALSE;
}

/**
 * Convert a UNIX timestamp to a date Campaign Monitor wants to receive.
 *
 * @param $timestamp
 *   Integer; The UNIX timestamp to convert.
 * @retun
 *   String; The Date in Campaign Monitor format.
 */
function _campaign_monitor_unix_to_cm_time($timestamp = 0) {
  if ($timestamp) {
    return date('Y-m-d H:i:s', $timestamp);
  }
  return 0;
}