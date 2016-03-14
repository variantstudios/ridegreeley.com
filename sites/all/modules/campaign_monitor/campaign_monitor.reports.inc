<?php
// $Id: campaign_monitor.reports.inc,v 1.1 2009/08/20 17:49:47 davyvandenbremt Exp $

/**
 * @file
 * Report functions and callbacks.
 *
 * @author Davy Van Den Bremt
 */

function campaign_monitor_reports_subscriptions() {
  $header = array(t('List'), t('Subscribers'));

  $result = db_query("SELECT cl.lid, cl.name_api, count(cs.mail) AS subscribers FROM {campaign_monitor_subscription} cs RIGHT JOIN {campaign_monitor_list} cl ON cl.lid = cs.lid GROUP BY cl.lid");

  $rows = array();

  while ($list = db_fetch_object($result)) {
    $rows[] = array(
      l($list->name_api, 'admin/build/campaign_monitor/'. $list->lid .'/manage'),
      $list->subscribers,
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No lists synchronised yet or no lists available in your account. Run cron to synchronise your Campaign Monitor account.'), 'colspan' => '4', 'class' => 'message'));
  }

  return theme('table', $header, $rows);
}

function campaign_monitor_reports_requests() {
  $header = array(t('Mail'), t('List'), t('Type'), t('Date'));

  $result = pager_query("SELECT * FROM {campaign_monitor_request} cr INNER JOIN {campaign_monitor_list} cl ON cl.lid = cr.lid ORDER BY cr.timestamp DESC");

  $rows = array();

  while ($request = db_fetch_object($result)) {
    $rows[] = array(
      $request->mail,
      l($request->name_api, 'admin/build/campaign_monitor/'. $request->lid .'/manage'),
      $request->type,
      t('!time ago', array('!time' => format_interval(time() - $request->timestamp))),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No requests pending.'), 'colspan' => '4', 'class' => 'message'));
  }

  return theme('table', $header, $rows) . theme('pager', array(), 50);
}