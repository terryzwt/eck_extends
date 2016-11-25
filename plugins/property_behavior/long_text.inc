<?php

/**
 * @file
 * Implementation of 'Long Text' property behavior plugin for eck.
 */

$plugin = array(
  'label' => 'Long Text (field replica)',
  'default_widget' => 'eckextend_long_text_property_widget',
  'default_formatter' => 'eckextend_long_text_property_formatter',
  'property_form' => 'eckextend_long_text_property_form',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_long_text_property_widget($property, $vars) {
  if ($settings = eckextend_property_settings($vars['entity']->entityType(), $property)) {
    return array(
      '#type' => 'textarea',
      '#cols' => $settings['cols'],
      '#rows' => $settings['rows'],
      '#required' => $settings['required'],
      '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
      '#default_value' => eckextend_get_property_value($vars['entity'], $property),
      '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
    );
  }
}

/**
 * Implements 'default_formatter' plugin.
 */
function eckextend_long_text_property_formatter($property, $vars) {
  $value = eckextend_get_property_value($vars['entity'], $property);
  $format_id = NULL;
  if ($settings = eckextend_property_settings($vars['entity']->entityType(), $property)) {
    if (isset($settings['text_format'])) {
      $format_id = $settings['text_format'];
    }
  }
  $safe_value = check_markup($value, $format_id);

  return eckextend_theme_property($safe_value, $property, $vars['entity']);
}

/**
 * Implements 'property_form' plugin.
 */
function eckextend_long_text_property_form($vars) {
  $settings = isset($vars['settings']) ? $vars['settings'] : NULL;
  // Property specific form.
  $form = array();
  $form['required'] = array(
    '#title' => t('Required property'),
    '#type' => 'checkbox',
    '#default_value' => isset($settings['required']) ? $settings['required'] : NULL,
  );
  $form['cols'] = array(
    '#title' => t('Columns'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#element_validate' => array('element_validate_integer_positive'),
    '#description' => t('How many columns wide the textarea should be.'),
    '#default_value' => isset($settings['cols']) ? $settings['cols'] : NULL,
  );
  $form['rows'] = array(
    '#title' => t('Rows'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#element_validate' => array('element_validate_integer_positive'),
    '#description' => t('How many rows high the textarea should be.'),
    '#default_value' => isset($settings['rows']) ? $settings['rows'] : NULL,
  );
  $format_options = array();
  foreach (filter_formats() as $name => $format) {
    $format_options[$name] = $format->name;
  }
  $form['text_format'] = array(
    '#title' => t('Text format'),
    '#type' => 'select',
    '#required' => TRUE,
    '#description' => t('The text format you would like to present to the user when creating this property. Plain text is the fallback format in case the user has no access to this text forma.'),
    '#options' => $format_options,
    '#default_value' => isset($settings['text_format']) ? $settings['text_format'] : NULL,
  );

  return $form;
}
