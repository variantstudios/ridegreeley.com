<?php
// $Id: campaign_monitor.block.inc,v 1.1.2.8 2009/09/13 05:30:03 davyvandenbremt Exp $

/**
 * @file
 * Blocks
 *
 * @author Davy Van Den Bremt
 */

function campaign_monitor_block_subscription_list_form($form, $lid = NULL, $hide_subscribed = TRUE) {
  drupal_add_css(drupal_get_path('module', 'campaign_monitor') .'/cm-style.css');

  $form = array();

  // fetching list object
  if ($lid) {
    $lists = array($lid);
  }
  else {
    $lists = campaign_monitor_db_get_lists(TRUE);
    $lists = array_keys($lists);
  }

  // if user is logged in, only allow subscription to lists not subscribed to yet
  if (!user_is_anonymous() && $hide_subscribed) {
    global $user;

    $result = db_query("SELECT lid FROM {campaign_monitor_subscription} WHERE mail = '%s'", $user->mail);

    $subscribed_lists = array();
    while ($row = db_fetch_object($result)) {
      $subscribed_lists[] = $row->lid;
    }

    $lists = array_diff($lists, $subscribed_lists);
  }

  if (count($lists) == 0) {
    return;
  }

  if (user_is_anonymous() || count($lists) == 1) {
    $list = campaign_monitor_db_get_list($lists[0]);

    if (!empty($list->name_action_long_clean)) {
      $form['action_long'] = array(
        '#type' => 'markup',
        '#value' => $list->name_action_long_clean,
      );
    }
  }

  if (user_is_anonymous()) {
    $form['fields']['#tree'] = TRUE;

    // mail field
    $form['fields']['mail'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail'),
      '#required' => TRUE,
    );

    // custom fields
    foreach ($lists as $list) {
      $list = campaign_monitor_db_get_list($list);

      foreach ($list->field_mapping as $field) {

        // don't show field in some occasions
        if (!$field->show_anonymous) continue;
        if ($lid == NULL && !$field->common_block_show) continue;
        if (in_array($field->key, array('mail'))) continue;

        // display field
        switch ($field->type) {
          case 'text':
            $form['fields'][$field->key] = array(
              '#type' => 'textfield',
              '#title' => t($field->name),
              '#weight' => $field->weight,
            );
            break;
          case 'select_one':
          case 'select_many':
            $form['fields'][$field->key] = array(
              '#type' => 'select',
              '#title' => t($field->name),
              '#options' => drupal_map_assoc($field->options),
              '#multiple' => $field->type == 'select_many',
              '#weight' => $field->weight,
            );
            break;
        }
      }
    }
  }

  if (!user_is_anonymous() || count($lists) > 1) {
    $options = array();
    foreach ($lists as $list) {
      $list = campaign_monitor_db_get_list($list);
      $options[$list->lid] = $list->name_action_clean;
    }

    $form['lists'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Newsletters'),
      '#options' => $options,
    );
  }
  else {
    // saving the list id in a hidden field
    $form['lists'] = array(
      '#type' => 'hidden',
      '#value' => $lists[0],
    );
  }

  // submit button
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Subscribe'),
  );

  return $form;
}

function campaign_monitor_block_subscription_list_form_validate($form, &$form_state) {
  if (user_is_anonymous()) {
    $mail = $form_state['values']['fields']['mail'];
    if (!valid_email_address($mail)) {
      form_set_error('mail', t('Not a valid e-mail address.'));
    }
  }
}

function campaign_monitor_block_subscription_list_form_submit($form, &$form_state) {
  $lists = array();

  if (is_array($form_state['values']['lists'])) {
    $lists = array_keys(array_filter($form_state['values']['lists']));
  }
  else {
    $lists[] = $form_state['values']['lists'];
  }

  foreach ($lists as $lid) {

    // fetch list info
    $list = campaign_monitor_db_get_list($lid);

    if (user_is_anonymous()) {
      // anonymous user
      $field_values = $form_state['values']['fields'];

      $mail = $field_values['mail'];
      unset($field_values['mail']);
    }
    else {
      // registered user
      $field_values = campaign_monitor_helpers_parse_field_values($list->lid);

      $mail = $field_values['mail'];

      unset($field_values['mail']);
    }

    // subscribe user
    campaign_monitor_helpers_subscribe($mail, $list->lid, $field_values);

    drupal_set_message(t('You have now been subscribed to the mailing list !list', array('!list' => $list->name_clean)));
  }
}