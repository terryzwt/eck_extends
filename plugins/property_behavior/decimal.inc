<?php

/**
 * @file
 * Implementation of 'Decimal' property behavior plugin for eck.
 */

$plugin = array(
  'label' => 'Decimal 18,2',
  'default_widget' => 'eckextend_decimal_property_widget',
  'default_formatter' => 'eckextend_property_default_formatter',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_decimal_property_widget($property, $vars) {
  return array(
    '#type' => 'textfield',
    '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
    '#default_value' => eckextend_get_property_value($vars['entity'], $property),
    '#required' => eckextend_get_property_required_info($vars['entity'], $property),
    '#element_validate' => array('element_validate_number', 'eckextend_null_property'),
    '#maxlength' => 18,
  );
}
