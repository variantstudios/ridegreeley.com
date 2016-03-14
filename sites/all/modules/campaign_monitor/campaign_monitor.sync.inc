<?php
// $Id: campaign_monitor.sync.inc,v 1.1.2.7 2009/09/11 20:12:39 davyvandenbremt Exp $

/**
 * @file
 * Cron synchronisation functions.
 *
 * @author Davy Van Den Bremt
 */

/**
 * Synchronise structure, requests and subscriptions (if allowed).
 */
function campaign_monitor_sync() {
  // fixme : what if a) request for subscription is done on site b) subscription is done somewhere else c) unsubscription is done somewhere else d) push of request is done

  // sync structure
  if (_campaign_monitor_allow_sync('structure')) {
    campaign_monitor_sync_lists();
    campaign_monitor_sync_fields();
  }

  // sync requests
  if (_campaign_monitor_allow_sync('requests')) {
    campaign_monitor_sync_push_requests();
  }

  // sync subscriptions
  if (_campaign_monitor_allow_sync('subscriptions')) {
    campaign_monitor_sync_subscriptions();
  }
}

/**
 * Synchronise mailing lists.
 */
function campaign_monitor_sync_lists() {
  $api_lists = campaign_monitor_api_get_lists();
  $db_lists = campaign_monitor_db_get_lists();

  $name = variable_get('campaign_monitor_default_name', '!name_api');
  $name_action = variable_get('campaign_monitor_default_name_action', 'Subscribe to !name');
  $name_action_long = variable_get('campaign_monitor_default_name_action_long', 'If you subscribe to !name, we will keep you up to date by sending newsletters.');
  $status = variable_get('campaign_monitor_default_status', TRUE);

  // check for new and missing lists
  if ($api_lists !== FALSE && $db_lists !== FALSE) {
    $added = array_diff_key($api_lists, $db_lists);
    $missing = array_diff_key($db_lists, $api_lists);

    foreach ($added as $list) {
      campaign_monitor_db_save_list($list->lid, $list->name_api, TRUE, $name, $status, $name_action, $name_action_long, '');
    }

    foreach ($missing as $lid => $list) {
      campaign_monitor_db_save_list($list->lid, $list->name_api, FALSE, $list->name, FALSE, $list->name_action, $list->name_action_long, $list->description);
    }
  }

  // check lists that were lost but now found again
  foreach ($db_lists as $list) {
    if ($list->status_api == FALSE && array_key_exists($list->lid, $api_lists)) {
      campaign_monitor_db_save_list($list->lid, $list->name_api, TRUE, $list->name, $list->status, $list->name_action, $list->name_action_long, $list->description);
    }
  }
}

/**
 * Synchronise fields of all lists that are avaiable in database.
 */
function campaign_monitor_sync_fields() {
  $lists = campaign_monitor_db_get_lists();

  foreach ($lists as $list) {
    $mapping = array();

    $fields = campaign_monitor_api_get_custom_fields($list->lid);

    // default field : mail
    if (!isset($list->field_mapping['mail'])) {
      $mapping['mail'] = new stdClass();
      if (module_exists('token')) {
        $mapping['mail']->mapping_type = 'token';
        $mapping['mail']->mapping_value = '[user-mail]';
      }
      else {
        $mapping['mail']->mapping_type = 'php';
        $mapping['mail']->mapping_value = '<?php global $user; return $user->mail; ?>';
      }
      $mapping['mail']->weight = 0;
    }
    else {
      $mapping['mail'] = $list->field_mapping['mail'];
    }
    $mapping['mail']->key = 'mail';
    $mapping['mail']->name = 'Mail';
    $mapping['mail']->type = 'text';
    $mapping['mail']->show_anonymous = TRUE;
    $mapping['mail']->common_block_show = TRUE;

    if ($fields) {

      // custom fields
      foreach ($fields as $field) {
        // this way we're deleting removed fields

        if (isset($list->field_mapping[$field->key])) {
          $mapping[$field->key] = $list->field_mapping[$field->key];
        }
        else {
          $mapping[$field->key] = new stdClass();
          $mapping[$field->key]->weight = 0;
        }
        $mapping[$field->key]->key = $field->key;
        $mapping[$field->key]->name = $field->name;
        $mapping[$field->key]->type = $field->type;
        if ($field->options) {
          $mapping[$field->key]->options = $field->options;
        }
      }
    }

    campaign_monitor_db_save_field_mapping($list->lid, $mapping);
  }
}

/**
 * Push all (un)subscription requests to Campaign Monitor.
 */
function campaign_monitor_sync_push_requests() {
  // fetch and count all requests
  $requests = array_values(campaign_monitor_db_get_requests());
  $count_requests = count($requests);

  // fetch lists
  $lists = campaign_monitor_db_get_lists();

  // init counter
  $count = 0;

  // loop through all requests ...
  while ($count < $count_requests) {
    // ... but if we have spent half of the maximum execution time, stop and wait 'till next cron run
    if ((timer_read('page') / 1000) > (ini_get('max_execution_time') / 2)) break;

    // fetch the request
    $request = $requests[$count];

    // fetch list for request
    $list = $lists[$request->lid];

    // fetch field mapping for list of request
    $mapping = $list->field_mapping;

    // only push fields that are still availabe on the list
    $fields = array_intersect_key($request->fields, $mapping);

    // push it
    $success = FALSE;
    if ($request->type == 'subscribe') {
      $success = campaign_monitor_api_subscribe($request->mail, $fields, $request->lid);
    }
    else {
      $success = campaign_monitor_api_unsubscribe($request->mail, $request->lid);
    }

    // if push succeeded, request can be deleted
    if ($success) {
      campaign_monitor_db_delete_request($request->mail, $request->lid);
    }

    // count up
    $count++;
  }
}

/**
 * Synchronise all subscriptions back from Campaign Monitor.
 */
function campaign_monitor_sync_subscriptions() {
  $last_sync = _campaign_monitor_unix_to_cm_time(variable_get('campaign_monitor_last_sync_subscriptions', 0));

  $lists = campaign_monitor_db_get_lists(TRUE);
  foreach ($lists as $lid => $list) {
    // fetch requests
    $requests = campaign_monitor_db_get_requests();

    // fetching subscriptions
    $subscribed = campaign_monitor_api_get_subscribers_subscribed($last_sync, $lid);
    $unsubscribed = campaign_monitor_api_get_subscribers_unsubscribed($last_sync, $lid);

    // errors?
    if ($subscribed === FALSE || $unsubscribed === FALSE) return;

    // check subscriptions
    if (count($subscribed) > 0) {
      foreach ($subscribed as $mail) {
        // if a request is pending for this address, ignore subscription status
        if (in_array($mail, $requests)) continue;

        // set address as subscripted to
        campaign_monitor_db_save_subscription($mail, $lid);
      }
    }

    // check unsubscriptions
    if (count($unsubscribed) > 0) {
      foreach ($unsubscribed as $mail) {
        // if a request is pending for this address, ignore subscription status
        if (in_array($mail, $requests)) continue;

        // delete address as subscripted to
        campaign_monitor_db_delete_subscription($mail, $lid);
      }
    }
  }
  variable_set('campaign_monitor_last_sync_subscriptions', $_SERVER['REQUEST_TIME']);
}