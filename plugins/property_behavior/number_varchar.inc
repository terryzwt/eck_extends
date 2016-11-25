<?php

/**
 * @file
 * Implementation of 'Number (varchar)' property behavior plugin for eck.
 */

// Plugin definition.
$plugin = array(
  'label' => 'Number (varchar)',
  'default_widget' => 'eckextend_number_varchar_property_widget',
  // Use the module simple default_formatter.
  'default_formatter' => 'eckextend_property_default_formatter',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_number_varchar_property_widget($property, $vars) {
  $entity = $vars['entity'];
  $value = eckextend_get_property_value($entity, $property);
  return array(
    '#type' => 'textfield',
    '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
    '#default_value' => $value,
    '#required' => eckextend_get_property_required_info($entity, $property),
    '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
    '#element_validate' => array('element_validate_number', 'eckextend_null_property'),
    '#maxlength' => 255,
  );
}
