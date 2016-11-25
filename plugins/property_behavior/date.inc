<?php

/**
 * @file
 * Implementation of 'Date' property behavior plugin for eck.
 */

if (module_exists('date_popup')) {
  $plugin = array(
    'label' => 'Date Popup',
    'default_widget' => 'eckextend_date_property_widget',
    'default_formatter' => 'eckextend_date_property_formatter',
    'property_info' => 'eckextend_date_property_info',
  );
}

/**
 * Implements 'property_info' plugin.
 */
function eckextend_date_property_info($property, $vars) {
  $vars['properties'][$property]['type'] = 'date';
  return $vars;
}

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_date_property_widget($property, $vars) {
  $entity = $vars['entity'];

  $value = _eckextend_date_extract($entity, $property, TRUE);

  $element = array(
    '#eck_date_property' => TRUE,
    '#type' => 'date_popup',
    '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
    '#default_value' => ($value == 0) ? '' : date('Y-m-d h:m:s', $value),
    '#date_year_range' => '-20:+3',
    '#date_format' => 'd/m/Y',
    '#date_timezone' => date_default_timezone(),
    '#date_flexible' => 0,
    '#datepicker_options' => array(),
    '#date_increment' => 1,
    '#theme_wrappers' => array('date_popup'),
    '#description' => t('date') . ' ' . t('in format') . ': ' . t('day') . '/' . t('month') . '/' . t('year') . '.',
    '#granularity' => array(
      'year' => 'year',
      'month' => 'month',
      'day' => 'day',
      'hour' => 0,
      'minute' => 0,
      'second' => 0,
    ),
  );

  return $element;
}

/**
 * Implements 'default_formatter' plugin.
 */
function eckextend_date_property_formatter($property, $vars) {
  $entity = $vars['entity'];
  $value = _eckextend_date_extract($entity, $property);
  return eckextend_theme_property($value, $property, $entity, 'date');
}

/**
 * Helper function to extract date property value and format it properly.
 */
function _eckextend_date_extract($entity, $property, $raw = FALSE) {
  $value = 0;
  if (isset($entity->{$property})) {
    if ($raw) {
      $value = $entity->{$property};
    }
    elseif ($value != 0) {
      $value = date('d/m/Y', $entity->{$property});
    }
  }

  return $value;
}
