<?php

/**
 * @file
 * Drush integration for the Revision Scheduler module.
 */

/**
 * Implements hook_drush_command().
 */
function revision_scheduler_drush_command() {
  $items = array();

  $items['revision-scheduler-process'] = array(
    'description' => 'Process all current revision scheduler items.',
    'callback' => 'drush_revision_scheduler_process',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  return $items;
}

/**
 * Implements drush_hook_COMMAND_validate().
 */
function drush_revision_scheduler_process_validate() {
  if (!variable_get('revision_scheduler_disable_cron', FALSE) && !lock_may_be_available('cron')) {
    return drush_set_error('CRON_RUNNING', dt('Blocked while cron is currently running.'));
  }
}

/**
 * Drush command callback for revision-scheduler-process.
 */
function drush_revision_scheduler_process() {
  // Add any scheduled items to process to the queue.
  revision_scheduler_cron();

  if (!DrupalQueue::get('revision_scheduler', TRUE)->numberOfItems()) {
    return;
  }

  // The process the items from the queue.
  if (module_exists('concurrent_queue')) {
    drush_invoke_process('@self', 'queue-run-concurrent', array('revision_scheduler'), array('concurrency' => 1));
  }
  else {
    drush_invoke_process('@self', 'queue-run', array('revision_scheduler'));
  }
}
