<?php

/**
 * @file
 * Implementation of 'Status' property behavior plugin for eck.
 */

// Plugin definition.
$plugin = array(
  'label' => t('Status'),
  'default_widget' => 'eckextend_status_property_widget',
  // Use the module simple default_formatter.
  'default_formatter' => 'eckextend_property_default_formatter',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_status_property_widget($property, $vars) {
  // Not checked by default.
  $val = isset($vars['entity']->{$property}) ? $vars['entity']->{$property} : 0;
  return array(
    '#type' => 'checkbox',
    '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
    '#default_value' => $val,
    '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
  );
}
