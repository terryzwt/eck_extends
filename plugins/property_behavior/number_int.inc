<?php

/**
 * @file
 * Implementation of 'Number (int)' property behavior plugin for eck.
 */

// Plugin definition.
$plugin = array(
  'label' => 'Number (int)',
  'default_widget' => 'eckextend_number_int_property_widget',
  // Use the module simple default_formatter.
  'default_formatter' => 'eckextend_property_default_formatter',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_number_int_property_widget($property, $vars) {
  return array(
    '#type' => 'textfield',
    '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
    '#default_value' => eckextend_get_property_value($vars['entity'], $property),
    '#required' => eckextend_get_property_required_info($vars['entity'], $property),
    '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
    '#element_validate' => array('element_validate_integer_positive', 'eckextend_null_property'),
    '#maxlength' => 11,
  );
}
