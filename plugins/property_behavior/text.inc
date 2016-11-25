<?php

/**
 * @file
 * Implementation of Text property behavior.
 */

$plugin = array(
  'label' => 'Text (field replica)',
  'default_widget' => 'eckextend_text_property_widget',
  'default_formatter' => 'eckextend_property_default_formatter',
  'property_form' => 'eckextend_text_property_form',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_text_property_widget($property, $vars) {
  if ($settings = eckextend_property_settings($vars['entity']->entityType(), $property)) {
    return array(
      '#type' => 'textfield',
      '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
      '#default_value' => eckextend_get_property_value($vars['entity'], $property),
      '#maxlength' => 255,
      '#required' => isset($settings['required']) ? $settings['required'] : FALSE,
      '#size' => isset($settings['size']) ? $settings['size'] : 60,
      '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
    );
  }
}

/**
 * Implements 'property_form' plugin.
 */
function eckextend_text_property_form($property, $vars) {
  $settings = isset($vars['settings']) ? $vars['settings'] : NULL;

  $form = eckextend_property_default_form($property, $vars);
  $form['size'] = array(
    '#title' => t('Size of text'),
    '#type' => 'textfield',
    '#default_value' => isset($settings['size']) ? $settings['size'] : 60,
    '#required' => TRUE,
  );

  return $form;
}