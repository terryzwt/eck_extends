<?php

/**
 * @file
 * Implementation of 'Textarea' property behavior plugin for eck.
 */

// Plugin definition.
$plugin = array(
  'label' => 'Textarea (deprecated)',
  'default_widget' => 'eckextend_textarea_property_widget',
  // Use the module simple default_formatter.
  'default_formatter' => 'eckextend_property_default_formatter',
  'property_form' => 'eckextend_textarea_property_form',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_textarea_property_widget($property, $vars) {
  return array(
    '#type' => 'textarea',
    '#cols' => 200,
    '#rows' => 10,
    '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
    '#default_value' => eckextend_get_property_value($vars['entity'], $property),
    '#required' => eckextend_get_property_required_info($vars['entity'], $property),
    '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
  );
}

/**
 * Implements 'property_form' plugin.
 */
function eckextend_textarea_property_form($vars) {
  $entity_type = $vars['entity_type'];
  $property = isset($vars['property']) ? $vars['property'] : NULL;
  // Property specific form.
  $form = array();
  // Required.
  $form['required'] = array(
    '#title' => t('Required property'),
    '#type' => 'checkbox',
  );
  // Widget type.
  $form['widget_type'] = array(
    '#title' => t('Widget Type'),
    '#type' => 'select',
    '#description' => t('The type of form element you would like to present to the user when creating this property.'),
    '#options' => array(
      'select' => t('Select list'),
      'checkboxes' => t('Check boxes'),
    ),
  );
  return $form;
}
