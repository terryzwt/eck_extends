<?php

/**
 * @file
 * Implementation of 'Textarea' property behavior plugin for eck.
 */

$plugin = array(
  'label' => 'List (field replica)',
  'default_widget' => 'eckextend_list_property_widget',
  'default_formatter' => 'eckextend_property_default_formatter',
  'property_form' => 'eckextend_list_property_form',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_list_property_widget($property, $vars) {
  if ($settings = eckextend_property_settings($vars['entity']->entityType(), $property)) {
    $options = isset($settings['allowed_values']) ? $settings['allowed_values'] : array();
    $element = array(
      '#type' => $settings['type'],
      '#options' => $settings['options'],
      '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
      '#default_value' => eckextend_get_property_value($vars['entity'], $property),
      '#required' => $settings['required'],
      '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
    );

    switch ($settings['type']) {
      // Select.
      case 'select':
        if (!$settings['required']) {
          $element['#empty_option'] = t('- None -');
        }
        else {
          $element['#empty_option'] = t('- Select a value -');
        }
        break;

      // Select.
      case 'radios':
        if (!$settings['required']) {
          $element['#options'] = array(NULL => t('N/A')) + $element['#options'];
        }
        break;
    }

    return $element;
  }
}

/**
 * Implements 'property_form' plugin.
 */
function eckextend_list_property_form($property, $vars) {
  module_load_include('inc', 'eckextend', 'plugins/property_behavior/list');
  $entity_type = $vars['entity_type'];
  $settings = isset($vars['settings']) ? $vars['settings'] : NULL;
  // Property specific form.
  $form = array();
  $form['required'] = array(
    '#title' => t('Required property'),
    '#type' => 'checkbox',
    '#default_value' => isset($settings['required']) ? $settings['required'] : NULL,
  );
  $desc = array(
    t('The possible values this property can contain. Enter one value per line, in the format key|label.'),
    t('The key is the stored value. The label will be used in displayed values and edit forms.'),
    t('The label is optional: if a line contains a single string, it will be used as key and label.'),
  );
  $form['options'] = array(
    '#title' => t('Options'),
    '#type' => 'textarea',
    '#element_validate' => array('eckextend_list_allowed_values_setting_validate'),
    '#description' => implode('<br />', $desc),
    '#required' => TRUE,
    '#default_value' => isset($settings['options']) ? _eckextend_list_options_string($settings['options']) : NULL,
  );
  $form['type'] = array(
    '#title' => t('Widget type (element)'),
    '#type' => 'select',
    '#description' => t('The type of form element you would like to present to the user when creating this property in the entity type form.'),
    '#options' => array(
      'select' => t('Select list'),
      'radios' => t('Radios'),
    ),
    '#default_value' => isset($settings['type']) ? $settings['type'] : NULL,
  );

  return $form;
}

