<?php

/**
 * @file Includes any rules integration provided by heartbeat.
 */


/**
 * Implements hook_rules_file_info().
 */
function heartbeat_rules_rules_file_info() {
  return array('heartbeat_rules');
}

/**
 * Implements hook_rules_data_info().
 */
function heartbeat_rules_rules_data_info() {
  return array(
    'heartbeat_activity' => array(
      'label' => t('Heartbeat template'),
      'parent' => 'entity',
      'group' => t('Heartbeat'),
    ),
  );
}

/**
 * Implements hook_rules_action_info().
 */
function heartbeat_rules_rules_action_info() {

  $base_parameters = array(
    'nid' => array('type' => 'token', 'label' => t('Entity ID')),
    'uid' => array('type' => 'token', 'label' => t('User ID')),
    'nid_target' => array('type' => 'token', 'label' => t('Entity target ID'), 'optional' => TRUE),
    'uid_target' => array('type' => 'token', 'label' => t('User target ID'), 'optional' => TRUE),
  );
  $actions = array();

  ctools_include('export');
  foreach (ctools_export_crud_load_all('heartbeat_messages', TRUE) as $template) {

    $variables = $template->variables;
    $name = 'heartbeat_activity_' . $template->message_id;

    $actions[$name] = array(
      'label' => t('Log activity for @template', array('@template' => $template->description)),
      'group' => t('Heartbeat'),
      'parameter' => $base_parameters,
      'base' => 'rules_action_heartbeat_activity_log',
      'callbacks' => array('form_alter' => 'heartbeat_rules_rules_action_form_alter'),
    );
    $actions[$name]['parameter']['message_id'] = array(
      'label' => t('Message ID'),
      'type' => 'list<text>',
      'options list' => 'heartbeat_templates_names',
      'default value' => $template->message_id,
      'description' => t('Unique identifier for the heartbeat message template that will be used.'),
    );

    if (!empty($variables)) {
      foreach ($variables as $var_name => $variable) {
        $actions[$name]['parameter'][$var_name] = array('type' => 'text', 'label' => $var_name);
      }
    }

  }

  return $actions;
}

function heartbeat_rules_rules_action_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  foreach (element_children($form['parameter']) as $field) {
    // Hack to move the dirty option field.
    // Note that at the time of writing of this code, rules did not export the
    // option lists other than "list<text>".
    if ($field == 'message_id') {
      $form['parameter'][$field]['#prefix'] = '<div style="display: none;">';
      $form['parameter'][$field]['#suffix'] = '</div>';
    }
    elseif ($form['parameter'][$field]['settings'][$field]['#type'] == 'textarea') {
      $form['parameter'][$field]['settings'][$field]['#type'] = 'textfield';
    }
  }
}
