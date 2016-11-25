<?php

/**
 * @file
 * Implementation of commerce_currency property behavior plugin for eck.
 */

if (!module_exists('commerce')) {
  $plugin = array(
    'label' => t('Commerce currency'),
    'default_widget' => 'eckextend_currency_property_widget',
    'default_formatter' => 'eckextend_property_default_formatter',
  );
}
/**
 * Implements 'default_widget' plugin.
 */
function eckextend_currency_property_widget($property, $vars) {
  foreach (commerce_currencies(TRUE) as $key => $value) {
    $options[$key] = $value['name'];
  }

  return array(
    '#type' => 'select',
    '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
    '#options' => $options,
    '#default_value' => eckextend_get_property_value($vars['entity'], $property),
    '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
  );
}
